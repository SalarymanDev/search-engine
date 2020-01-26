#ifndef ISR_H
#define ISR_H

#include <cstddef>
#include "src/libs/index/PostingList/PostingList.h"
#include "src/libs/vector/vector.h"
#include "src/libs/logger/logger.h"
#include "src/libs/index/rankingData/rankingData.h"


namespace BBG {

   // This struct is returned by the ISRs.
   // @param docEnd can be used to retrieve the url and html for the document.
   // @param wordFeatures is the size of the query size, and each element contains
   //    all the relavant features for dynamic ranking.
   struct DocumentMatch {
      EndDocPost* docEnd;
      vector<WordFeatures*> wordFeatures;
      vector<WordDocFeatures*> wordDocFeatures;
      vector<float> htmlTags;
   };

   class ISR {
   public:
      Logger log;

      virtual ~ISR() = 0;
      virtual DocumentMatch NextDocument() = 0;
      virtual DocumentMatch extractFeatures() = 0;
      virtual uint32_t GetStartLocation() = 0;
      virtual uint32_t GetEndLocation() = 0;
      virtual EndDocPost* Seek(uint32_t target) = 0;
   };

   class ISRWord : public ISR {
   public:
      ISRWord(PostingList* _pList, PostingList* _endDocPList);

      virtual DocumentMatch NextDocument();
      virtual EndDocPost* Seek(uint32_t target);
      virtual DocumentMatch extractFeatures();

      // unsigned GetDocumentCount();
      // unsigned GetNumberOfOccurrences();
      // virtual Post* GetCurrentPost();
      virtual uint32_t GetStartLocation();
      virtual uint32_t GetEndLocation();
   
   private:
      PostingList* pList;
      PostingList* endDocPList;
      Post* currentPost;
      // Post* lastPost; // possible optimizations
      // EndDocPost* lastDocumentEnd;
      EndDocPost* currentDocumentEnd;
      bool init;
   };

   class ISROr : public ISR {
   private:
      uint32_t nearestTerm;
      EndDocPost* currentDocumentEnd;
      bool init;
      vector<ISR*> terms;
   
   public:
      ISROr();
      ~ISROr();

      virtual uint32_t GetStartLocation();
      virtual uint32_t GetEndLocation();
      virtual DocumentMatch extractFeatures();

      // Seek all the ISRs to the first occurrence just past
      // the end of this document.
      virtual DocumentMatch NextDocument();

      // Seek all the ISRs to the first occurrence beginning at
      // the target location. Return null if there is no match.
      // The document is the document containing the nearest term.
      virtual EndDocPost* Seek(uint32_t target);

      void addTerm(ISR* term);
   };

   class ISRAnd : public ISR {
   private:
      EndDocPost* currentDocumentEnd;
      bool init;
      vector<ISR*> terms;
   
   public:
      ISRAnd();
      ~ISRAnd();

      virtual DocumentMatch NextDocument();
      virtual DocumentMatch extractFeatures();

      // 1. Seek all the ISRs to the first occurrence beginning at
      // the target location.
      // 2. Move the document end ISR to just past the furthest
      // word, then calculate the document begin location.
      // 3. Seek all the other terms to past the document begin.
      // 4. If any term is past the document end, return to
      // step 2.
      // 5. If any ISR reaches the end, there is no match.
      virtual EndDocPost* Seek(uint32_t target);

      virtual uint32_t GetStartLocation();
      virtual uint32_t GetEndLocation();

      void addTerm(ISR* term);
   };
}

#endif
