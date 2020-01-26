cc_library(
    name = "preprocessing",
    hdrs = [
        "BagOfWordsProcessor.h",
        "TermFrequencyProcessor.h",
        "InverseDocumentFrequencyProcessor.h",
        "NormalizationProcessor.h",
        "AnchorTextProcessor.h",
    ],
    srcs = [
        "BagOfWordsProcessor.cc",
        "TermFrequencyProcessor.cc",
        "InverseDocumentFrequencyProcessor.cc",
        "NormalizationProcessor.cc",
        "AnchorTextProcessor.cc",
    ],
    deps = [
        "//src/libs/preprocessing/loaders",
        "//src/libs/vector",
        "//src/libs/string",
        "//src/libs/map",
        "//src/libs/serialize",
        "//src/libs/document",
        "//src/libs/gdfs",
        "//src/libs/logger",
        "//src/libs/preprocessedDocument"
    ],
    visibility = ["//visibility:public"],
)