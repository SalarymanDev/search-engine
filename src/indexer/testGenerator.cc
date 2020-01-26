#include "src/libs/document/Document.h"
#include "src/libs/gdfs/gdfs.h"

using namespace BBG;

int main(int argc, char** argv) {
    Document doc1;
    doc1.url = "https://google.com/";
    doc1.imgCount = 2;
    doc1.size = 1500;

    doc1.bodyText.push_back("google");
    doc1.bodyText.push_back("search");
    doc1.bodyText.push_back("lucky");
    
    doc1.titleText.push_back("google");

    doc1.anchors["https://robertkennedy.me/"].push_back("robert");
    doc1.anchors["https://robertkennedy.me/"].push_back("kennedy");
    doc1.anchors["https://robertkennedy.me/"].push_back("resume");
    doc1.anchors["https://nicolehamilton.com/"].push_back("eecs");
    doc1.anchors["https://nicolehamilton.com/"].push_back("398");

    Document doc2;
    doc2.url = "https://robertkennedy.me/";
    doc2.imgCount = 7;
    doc2.size = 7000;

    doc2.bodyText.push_back("robert");
    doc2.bodyText.push_back("kennedy");
    doc2.bodyText.push_back("computer");
    doc2.bodyText.push_back("science");
    
    doc2.titleText.push_back("robert");
    doc2.titleText.push_back("kennedy");

    doc2.anchors["https://nicolehamilton.com/"].push_back("eecs");
    doc2.anchors["https://nicolehamilton.com/"].push_back("398");
    doc2.anchors["https://amazon.com/"].push_back("amazon");
    doc2.anchors["https://imagesoftinc.com/"].push_back("amazon");

    Document doc3;
    doc3.url = "https://nicolehamilton.com/";
    doc3.imgCount = 3;
    doc3.size = 3000;

    doc3.bodyText.push_back("nicole");
    doc3.bodyText.push_back("hamilton");
    doc3.bodyText.push_back("eecs");
    doc3.bodyText.push_back("398");
    doc3.bodyText.push_back("search");
    doc3.bodyText.push_back("engines");
    
    doc3.titleText.push_back("nicole");
    doc3.titleText.push_back("hamilton");

    doc3.anchors["https://robertkennedy.me/"].push_back("student");
    doc3.anchors["https://google.com/"].push_back("google");

    {
        GDFS fs;
        fs.addDocument(doc1);
        fs.addDocument(doc2);
        fs.addDocument(doc3);
    }

    return 0;
}