#ifndef QMPACK_H
#define QMPACK_H

enum asset_type {
    ASSET_TYPE_OBJ,
    ASSET_TYPE_BMP,
    ASSET_TYPE_ATLAS_TEXTURE,
    ASSET_TYPE_ATLAS_DATA,
    ASSET_TYPE_ANIMATION_DATA,
    ASSET_TYPE_QMM,
    ASSET_TYPE_WAV,
    ASSET_TYPE_MIDI,
    ASSET_TYPE_DATA
};

enum data_type {
    DATA_TYPE_BINARY,
    DATA_TYPE_TEXT
};

#pragma pack(push, 1)
struct qmpack_file_header {
    char name[50];
    unsigned int size;
    asset_type type;
};
#pragma pack(pop)

struct qmpack_staging_data {
    qmpack_file_header fileHeader;
    char *filePath;
    data_type dataType;
};

#endif
