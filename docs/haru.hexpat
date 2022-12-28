struct String {
    u8 length;
    char string[length];
};

struct KeyValue {
    String key;
    String value;
};

struct Vec3 {
    float x;
    float y;
    float z;
};

struct Vec2 {
    float x;
    float y;
};

struct Vertex {
    Vec3 position;
    Vec2 texCoord;
};

struct Face {
    String texture;
    Vec3 normal;
    u16 numVertices;
    Vertex vertices[numVertices];
};

struct Brush {
    u16 numVertices;
    Vec3 vertices[numVertices];

    u16 numFaces;
    Face faces[numFaces];
};

struct Entity {
    u16 numProperties;
    KeyValue properties[numProperties];
    u16 numBrushes;
    Brush brushes[numBrushes];
};

struct Map {
    u16 numEntities;
    Entity entities[numEntities];
};

Map map @0x00;
