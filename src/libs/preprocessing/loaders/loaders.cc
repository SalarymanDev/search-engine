#include "src/libs/preprocessing/loaders/loaders.h"
#include "src/libs/serialize/Buffer.h"
#include "src/libs/logger/logger.h"

using BBG::Buffer;
using BBG::Logger;

Logger logger;

bool BBG::loadBow(const char *filename, Map<string, uint32_t>& bow_out) {
    // Handles deserializtion from backup file
    FILE* file = fopen(filename, "rb");

    if (file) {
        fseek(file, 0, SEEK_END);
        size_t filesize = ftell(file);
        rewind(file);
        
        Buffer bytes(filesize);
        if (fread(bytes.modifyData(), 1, bytes.size(), file) == -1)
            logger.Fatal("[Loaders] failed to read from file: " + string(filename));
        fclose(file);

        deserialize(bytes, bow_out);
        return true;
    } else {
        logger.Info("BOW file doesn't exist");
        return false;
    }
}

bool BBG::loadTF(const char *filename, Map<uint32_t, Map<string, uint32_t>> &tf_out) {
    // Handles deserializtion from backup file
    FILE* file = fopen(filename, "rb");

    if (file) {
        fseek(file, 0, SEEK_END);
        size_t filesize = ftell(file);
        rewind(file);
        
        Buffer bytes(filesize);
        if (fread(bytes.modifyData(), 1, bytes.size(), file) == -1)
            logger.Fatal("[Loaders] failed to read from file: " + string(filename));
        fclose(file);

        deserialize(bytes, tf_out);
        return true;
    } else {
        logger.Info("TF file doesn't exist");
        return false;
    }
}

bool BBG::loadIDF(const char *filename, Map<uint32_t, float>& IDF) {
    // Handles deserializtion from backup file
    FILE* file = fopen(filename, "rb");

    if (file) {
        fseek(file, 0, SEEK_END);
        size_t filesize = ftell(file);
        rewind(file);
        
        Buffer bytes(filesize);
        if (fread(bytes.modifyData(), 1, bytes.size(), file) == -1)
            logger.Fatal("[Loaders] failed to read from file: " + string(filename));
        fclose(file);

        deserialize(bytes, IDF);
        return true;
    } else {
        logger.Info("IDF file doesn't exist");
        return false;
    }
}

bool BBG::loadNorm(const char *filename, Map<string, float>& NORM) {
    // Handles deserializtion from backup file
    FILE* file = fopen(filename, "rb");

    if (file) {
        fseek(file, 0, SEEK_END);
        size_t filesize = ftell(file);
        rewind(file);
        
        Buffer bytes(filesize);
        if (fread(bytes.modifyData(), 1, bytes.size(), file) == -1)
            logger.Fatal("[Loaders] failed to read from file: " + string(filename));
        fclose(file);

        deserialize(bytes, NORM);
        return true;
    } else {
        logger.Info("NORM file doesn't exist");
        return false;
    }
}
