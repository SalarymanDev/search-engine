#include "src/libs/ISR/ISR.h"

using BBG::ISRWord;
using BBG::ISRAnd;
using BBG::ISROr;
using BBG::Post;
using BBG::EndDocPost;
using BBG::PostingList;
using BBG::DocumentMatch;

inline ISR::~ISR() {}

ISRWord::ISRWord(PostingList* _pList, PostingList* _endDocPList)
   : pList(_pList), endDocPList(_endDocPList) {
   init = true;
   if (!pList) {
      init = false;
      currentDocumentEnd = nullptr;
      log.Debug("[ISRWord::ISRWord] PostingList not found in index chunk");
   }
   if (!endDocPList)
      log.Fatal("[ISRWord::ISRWord] EndDoc PostingList not found in index chunk");
}

DocumentMatch ISRWord::extractFeatures() {
   DocumentMatch result;
   result.docEnd = currentDocumentEnd;
   if (currentDocumentEnd) {
      log.Debug("[ISRWord] Next url match is " + currentDocumentEnd->getUrl() +
                " (doc end location " + to_string(currentDocumentEnd->getAbsoluteLocation()) + ")");
      result.wordFeatures.push_back(&(pList->wordFeatures));
      result.wordDocFeatures.push_back(pList->getWordDocFeature(currentDocumentEnd));
      result.htmlTags.push_back((float)currentPost->getHtmlTag());
   }

   return result;
}

DocumentMatch ISRWord::NextDocument() {
   if (init) {
      init = false;
      currentPost = pList->Seek(0);
      currentDocumentEnd = currentPost ? endDocPList->SeekEndDoc(currentPost->getAbsoluteLocation()) : nullptr;
   }
   else if (currentDocumentEnd)
      Seek(currentDocumentEnd->getAbsoluteLocation() + 1);

   return extractFeatures();
}

EndDocPost* ISRWord::Seek(uint32_t target) {
   log.Debug("[ISRWord] Seeking ISR to location " + to_string(target));

   if (!pList) {
      currentDocumentEnd = nullptr;
      log.Debug("[ISRWord::ISRWord] PostingList not found in index chunk");
      return nullptr;
   }
   if (!endDocPList)
      log.Fatal("[ISRWord::ISRWord] EndDoc PostingList not found in index chunk");

   currentPost = pList->Seek(target);
   currentDocumentEnd = currentPost ? endDocPList->SeekEndDoc(currentPost->getAbsoluteLocation()) : nullptr;

   if (currentDocumentEnd)
      log.Debug("[ISRWord] Current document end is now " + to_string(currentDocumentEnd->getAbsoluteLocation()));
   else
      log.Debug("[ISRWord] No more matches");
   
   return currentDocumentEnd;
}

uint32_t ISRWord::GetStartLocation() {
   return currentDocumentEnd->docStart();
}

uint32_t ISRWord::GetEndLocation() {
   return currentDocumentEnd->getAbsoluteLocation();
}


ISRAnd::ISRAnd() : init(true) {}

ISRAnd::~ISRAnd() {
   for (size_t i = 0; i < terms.size(); ++i)
      delete terms[i];
}

void ISRAnd::addTerm(ISR* term) {
   if (term)
      terms.push_back(term);
}

DocumentMatch ISRAnd::extractFeatures() {
   DocumentMatch result, tmp;
   result.docEnd = currentDocumentEnd;

   if (currentDocumentEnd) {
      for (uint32_t i = 0; i < terms.size(); ++i) {
         tmp = terms[i]->extractFeatures();
         for (uint32_t j = 0; j < tmp.wordFeatures.size(); ++j) {
            result.wordFeatures.push_back(tmp.wordFeatures[j]);
            result.wordDocFeatures.push_back(tmp.wordDocFeatures[j]);
            result.htmlTags.push_back(tmp.htmlTags[j]);
         }
      }
      log.Debug("[ISRAnd] Next url match is " + currentDocumentEnd->getUrl() +
             " (doc end location " + to_string(currentDocumentEnd->getAbsoluteLocation()) + ")");
   }
   
   return result;
}

DocumentMatch ISRAnd::NextDocument() {
   if (init) {
      init = false;
      Seek(0);
   }
   else if (currentDocumentEnd)
      Seek(currentDocumentEnd->getAbsoluteLocation() + 1);
      
   return extractFeatures();
}

EndDocPost* ISRAnd::Seek(uint32_t target) {
   log.Debug("[ISRAnd] Seek");
   currentDocumentEnd = terms[0]->Seek(target);
   if (!currentDocumentEnd) {
      log.Debug("[ISRAnd] No more matches");
      return nullptr;
   }

   bool alltermsInDoc;
   uint32_t end;
   uint32_t targetEnd = currentDocumentEnd->getAbsoluteLocation();
   target = terms[0]->GetStartLocation();

   do {
      log.Debug("[ISRAnd] Seeking all child ISRs to location " + to_string(target));
      alltermsInDoc = true;
      for (uint32_t i = 0; i < terms.size(); ++i) {
         currentDocumentEnd = terms[i]->Seek(target);
         if (!currentDocumentEnd) {
            log.Debug("[ISRAnd] No more matches");
            return nullptr;
         }
         end = currentDocumentEnd->getAbsoluteLocation();
         if (end != targetEnd) {
            alltermsInDoc = false;
            targetEnd = end;
            target = terms[i]->GetStartLocation();
            log.Debug("[ISRAnd] Term " + to_string(i) + "has no martches starting at " +
               to_string(target) + ". Setting new seek target");
            break;
         }
      }
   } while (!alltermsInDoc);
   
   log.Debug("[ISRAnd] Current document end is now " + to_string(currentDocumentEnd->getAbsoluteLocation()));

   return currentDocumentEnd;
}

uint32_t ISRAnd::GetStartLocation() {
   return terms[0]->GetStartLocation();
}

uint32_t ISRAnd::GetEndLocation() {
   return currentDocumentEnd->getAbsoluteLocation();
}

ISROr::ISROr() : nearestTerm(0), init(true) {}

ISROr::~ISROr() {
   for (size_t i = 0; i < terms.size(); ++i)
      delete terms[i];
}

void ISROr::addTerm(ISR* term) {
   if (term)
      terms.push_back(term);
}

DocumentMatch ISROr::NextDocument() {
   if (init) {
      init = false;
      Seek(0);
   }
   else if (currentDocumentEnd)
      Seek(currentDocumentEnd->getAbsoluteLocation() + 1);
   return extractFeatures();
}

DocumentMatch ISROr::extractFeatures() {
   DocumentMatch result;
   result.docEnd = currentDocumentEnd;

   if (currentDocumentEnd) {
      log.Debug("[ISROr] Next url match is " + currentDocumentEnd->getUrl() +
                " (doc end location " + to_string(currentDocumentEnd->getAbsoluteLocation()) + ")");
      return terms[nearestTerm]->extractFeatures();
   }
   
   return result;
}
  

EndDocPost* ISROr::Seek(uint32_t target) {
   log.Debug("[ISROr] Seeking ISR to location " + to_string(target));

   currentDocumentEnd = nullptr;
   EndDocPost* tmp = nullptr;
   uint32_t closest = UINT32_MAX;

   for (size_t i = 0; i < terms.size(); ++i) {
      tmp = terms[i]->Seek(target);
      if (tmp && terms[i]->GetEndLocation() < closest) {
         nearestTerm = i;
         currentDocumentEnd = tmp;
         closest = terms[i]->GetEndLocation();
      }
   }

   if (currentDocumentEnd)
      log.Debug("[ISROr] Current document end is now " + to_string(currentDocumentEnd->getAbsoluteLocation()));
   else
      log.Debug("[ISROr] No more matches");
   
   return currentDocumentEnd;
}

uint32_t ISROr::GetStartLocation() {
   return terms[nearestTerm]->GetStartLocation();
}

uint32_t ISROr::GetEndLocation() {
   return currentDocumentEnd->getAbsoluteLocation();
}
