
#if 0
static bool sameSign (float a, float b) {
    bool aNegative = a < 0.0f;
    bool bNegative = b < 0.0f;
    return aNegative == bNegative;
}

// From RTCD
static bool lineSegmentTriangleIntersection (line *lineSegment, triangleWithNormals *tri, float *t, vector3 *normal) {
    vector3 pq = lineSegment->b - lineSegment->a;
    vector3 pa = tri->p0 - lineSegment->a;
    vector3 pb = tri->p1 - lineSegment->a;
    vector3 pc = tri->p2 - lineSegment->a;

    vector3 m = crossProduct(pq, pc);
    float u = dotProduct(pb, m);
    float v = -dotProduct(pa, m);
    if (!sameSign(u, v)) { return false; }
    float w = dotProduct(crossProduct(pq, pb), pa);
    if (!sameSign(u, w)) { return false; }

    float denom = 1.0f / (u + v + w);
    u *= denom;
    v *= denom;
    w *= denom;

    vector3 r = u*tri->p0 + v*tri->p1 + w*tri->p2;
    vector3 ab = lineSegment->b - lineSegment->a;
    *t = dotProduct(r - lineSegment->a, ab) / dotProduct(ab, ab);

    *normal = normalize(u * tri->n0 + v * tri->n1 + w * tri->n2);

    if (*t >= 0.0f && *t <= 1.0f) { return true; }
    return false;
}
#endif
                                        
static void *pushRenderCommand (render_command_list *renderCommands, 
                         render_command_type type, 
                         unsigned int size) 
{
    void *renderCommand = 0;
    int requiredSize = sizeof(render_command_header) + size;
    if (renderCommands->memory.size + requiredSize < renderCommands->memory.capacity) {
        render_command_header *header = (render_command_header *)((char *)renderCommands->memory.base + 
                                                                  renderCommands->memory.size);
        renderCommands->memory.size += requiredSize;
        header->type = type;
        renderCommand = (char *)header + sizeof(render_command_header);
    }
    else {
        // Out of memory error?
    }
    return renderCommand;
}




static line *pushLineOntoRenderCommand (render_command_list *renderCommands, render_command_lines *lineCommand) {
    assert(renderCommands->memory.size + sizeof(line) < renderCommands->memory.capacity);

    line *result = (line *)((char *)renderCommands->memory.base + renderCommands->memory.size);
    renderCommands->memory.size += sizeof(line);

    // must finish with the line command before doing other ones
    assert(result == lineCommand->lines + lineCommand->numLines);
    lineCommand->numLines++;

    return result;
}

#if 0
static void addSprite (sprite newSprite, sprite_list *spriteList) {
    assert(spriteList->numSprites < MAX_SPRITES_PER_FRAME);

    sprite *nextSprite = &spriteList->sprites[spriteList->numSprites];
    ++spriteList->numSprites;
    *nextSprite = newSprite;
}
#endif

static void drawModel (char *meshKey, char *textureKey, matrix4x4 modelMatrix, 
                       render_command_list *renderCommands, game_assets *assets) 
{
    texture_asset *texAsset = getTexture(assets, textureKey);
    unsigned int textureID = texAsset->id;
    mesh_asset *meshAsset = getMesh(assets, meshKey);
    unsigned int meshID = meshAsset->id;
    render_command_model *modelCommand = 
        (render_command_model *)pushRenderCommand(renderCommands,
                                                 RENDER_COMMAND_MODEL,
                                                 sizeof(render_command_model));
    modelCommand->meshID = meshID;
    modelCommand->textureID = textureID;
    modelCommand->modelMatrix = modelMatrix;
}

static void drawDynamicModel (unsigned int meshID, char *textureKey, matrix4x4 modelMatrix, 
                              render_command_list *renderCommands, game_assets *assets) 
{
    texture_asset *texAsset = getTexture(assets, textureKey);
    unsigned int textureID = texAsset->id;

    // TODO(ebuchholz): maybe use a flag to indicate the mesh is one of the dynamic ones, rather than
    // create a new render command for it
    render_command_dynamic_model *modelCommand = 
        (render_command_dynamic_model *)pushRenderCommand(renderCommands,
                                                 RENDER_COMMAND_DYNAMIC_MODEL,
                                                 sizeof(render_command_dynamic_model));
    modelCommand->meshID = meshID;
    modelCommand->textureID = textureID;
    modelCommand->modelMatrix = modelMatrix;
}

static void generateDynamicModel (unsigned int meshID, 
                                  float_mesh_attribute positions, 
                                  float_mesh_attribute texCoords,
                                  float_mesh_attribute normals,
                                  int_mesh_attribute indices, render_command_list *renderCommands) 
{
    render_command_generate_mesh *generateMeshCommand = 
        (render_command_generate_mesh *)pushRenderCommand(renderCommands,
                                                 RENDER_COMMAND_GENERATE_MESH,
                                                 sizeof(render_command_generate_mesh));
    // TODO(ebuchholz): determine if all the data should be allocated from "render" memory or if
    // temp memory is fine
    generateMeshCommand->id = meshID;
    generateMeshCommand->positions = positions;
    generateMeshCommand->texCoords = texCoords;
    generateMeshCommand->normals = normals;
    generateMeshCommand->indices = indices;
}

static void drawAnimatedModel (animated_mesh_key meshKey, char *textureKey, 
                               matrix4x4 modelMatrix, 
                               skeleton_bone *bones, matrix4x4 *inverseTransforms, int numBones, 
                               render_command_list *renderCommands, game_assets *assets) 
{
    render_command_animated_model *animatedModelCommand = 
        (render_command_animated_model *)pushRenderCommand(renderCommands,
                                                 RENDER_COMMAND_ANIMATED_MODEL,
                                                 sizeof(render_command_animated_model));

    texture_asset *texAsset = getTexture(assets, textureKey);
    unsigned int textureID = texAsset->id;

    animatedModelCommand->animatedMeshKey = meshKey;
    animatedModelCommand->textureID = textureID;
    animatedModelCommand->modelMatrix = modelMatrix;

    assert(renderCommands->memory.size + sizeof(matrix4x4) * numBones < renderCommands->memory.capacity);

    matrix4x4 *boneMatrices = (matrix4x4 *)((char *)renderCommands->memory.base + renderCommands->memory.size);
    renderCommands->memory.size += sizeof(matrix4x4) * numBones;
    animatedModelCommand->boneMatrices = boneMatrices;
    animatedModelCommand->numBones = numBones;

    for (int i = 0; i < numBones; ++i) {
        animatedModelCommand->boneMatrices[i] = bones[i].transform * inverseTransforms[i];
    }
}


static render_command_lines *startLines (render_command_list *renderCommands) {
    render_command_lines *lineCommand = 
        (render_command_lines *)pushRenderCommand(renderCommands, RENDER_COMMAND_LINES, sizeof(render_command_lines));
    lineCommand->lines = (line *)((char *)renderCommands->memory.base + renderCommands->memory.size);
    lineCommand->numLines = 0; // zero this out: was a bug where the value from the last frame persisted
    return lineCommand;
}

static void drawLine (float ax, float ay, float az, float bx, float by, float bz, render_command_list *renderCommands, render_command_lines *lineCommand) {
    line *line = pushLineOntoRenderCommand(renderCommands, lineCommand);
    line->a.x = ax;
    line->a.y = ay;
    line->a.z = az;
    line->b.x = bx;
    line->b.y = by;
    line->b.z = bz;
}

static void drawViewFrustum (vector3 camPos, quaternion camRotation, float nearDist, float farDist, 
                             float fov, float viewRatio, render_command_list *renderCommands) 
{
    render_command_lines *lineCommand = startLines(renderCommands);

    frustum_corners corners = FrustumCorners(camPos, camRotation, nearDist, farDist, fov, viewRatio);

    drawLine(corners.nearUpperLeft.x, corners.nearUpperLeft.y, corners.nearUpperLeft.z, 
             corners.nearUpperRight.x, corners.nearUpperRight.y, corners.nearUpperRight.z, renderCommands, lineCommand);
    drawLine(corners.nearUpperRight.x, corners.nearUpperRight.y, corners.nearUpperRight.z, 
             corners.nearLowerRight.x, corners.nearLowerRight.y, corners.nearLowerRight.z, renderCommands, lineCommand);
    drawLine(corners.nearLowerRight.x, corners.nearLowerRight.y, corners.nearLowerRight.z, 
             corners.nearLowerLeft.x, corners.nearLowerLeft.y, corners.nearLowerLeft.z, renderCommands, lineCommand);
    drawLine(corners.nearLowerLeft.x, corners.nearLowerLeft.y, corners.nearLowerLeft.z, 
             corners.nearUpperLeft.x, corners.nearUpperLeft.y, corners.nearUpperLeft.z, renderCommands, lineCommand);

    drawLine(corners.farUpperLeft.x, corners.farUpperLeft.y, corners.farUpperLeft.z, 
             corners.farUpperRight.x, corners.farUpperRight.y, corners.farUpperRight.z, renderCommands, lineCommand);
    drawLine(corners.farUpperRight.x, corners.farUpperRight.y, corners.farUpperRight.z, 
             corners.farLowerRight.x, corners.farLowerRight.y, corners.farLowerRight.z, renderCommands, lineCommand);
    drawLine(corners.farLowerRight.x, corners.farLowerRight.y, corners.farLowerRight.z, 
             corners.farLowerLeft.x, corners.farLowerLeft.y, corners.farLowerLeft.z, renderCommands, lineCommand);
    drawLine(corners.farLowerLeft.x, corners.farLowerLeft.y, corners.farLowerLeft.z, 
             corners.farUpperLeft.x, corners.farUpperLeft.y, corners.farUpperLeft.z, renderCommands, lineCommand);

    drawLine(corners.nearUpperLeft.x, corners.nearUpperLeft.y, corners.nearUpperLeft.z, 
             corners.farUpperLeft.x, corners.farUpperLeft.y, corners.farUpperLeft.z, renderCommands, lineCommand);
    drawLine(corners.nearUpperRight.x, corners.nearUpperRight.y, corners.nearUpperRight.z, 
             corners.farUpperRight.x, corners.farUpperRight.y, corners.farUpperRight.z, renderCommands, lineCommand);
    drawLine(corners.nearLowerRight.x, corners.nearLowerRight.y, corners.nearLowerRight.z, 
             corners.farLowerRight.x, corners.farLowerRight.y, corners.farLowerRight.z, renderCommands, lineCommand);
    drawLine(corners.nearLowerLeft.x, corners.nearLowerLeft.y, corners.nearLowerLeft.z, 
             corners.farLowerLeft.x, corners.farLowerLeft.y, corners.farLowerLeft.z, renderCommands, lineCommand);
}

#if 0
// same as drawTriangle but without making a new render command
//static void drawMeshTriangle (triangle *t, render_command_list *renderCommands, render_command_lines *lineCommand) {
//    drawLine(t->p0.x, t->p0.y, t->p0.z, t->p1.x, t->p1.y, t->p1.z, renderCommands, lineCommand);
//    drawLine(t->p1.x, t->p1.y, t->p1.z, t->p2.x, t->p2.y, t->p2.z, renderCommands, lineCommand);
//    drawLine(t->p2.x, t->p2.y, t->p2.z, t->p0.x, t->p0.y, t->p0.z, renderCommands, lineCommand);
//}

//static void drawTriangle (triangle *t, render_command_list *renderCommands) {
//    render_command_lines *lineCommand = startLines(renderCommands);
//    drawLine(t->p0.x, t->p0.y, t->p0.z, t->p1.x, t->p1.y, t->p1.z, renderCommands, lineCommand);
//    drawLine(t->p1.x, t->p1.y, t->p1.z, t->p2.x, t->p2.y, t->p2.z, renderCommands, lineCommand);
//    drawLine(t->p2.x, t->p2.y, t->p2.z, t->p0.x, t->p0.y, t->p0.z, renderCommands, lineCommand);
//}

//static void drawLevelMesh (level_mesh *levelMesh, render_command_list *renderCommands) {
//    render_command_lines *lineCommand = startLines(renderCommands);
//    triangle *triangles = levelMesh->triangles;
//    for (int i = 0; i < levelMesh->triangleCount; ++i) {
//        drawMeshTriangle(triangles + i, renderCommands, lineCommand);
//    }
//}

//static void drawCircle (float centerX, float centerY, float centerZ, float radius, render_command_list *renderCommands) {
//    render_command_lines *lineCommand = startLines(renderCommands);
//    const int numPoints = 16;
//    float deltaAngle = (2.0f * PI) / numPoints;
//    for (int i = 0; i < numPoints; ++i) {
//        drawLine(centerX + radius * cosf(deltaAngle * (float)i), centerY + radius * sinf(deltaAngle * (float)i), centerZ, 
//                 centerX + radius * cosf(deltaAngle * (float)((i+1)%numPoints)), centerY + radius * sinf(deltaAngle * (float)((i+1)%numPoints)), centerZ, 
//                 renderCommands, lineCommand);
//    }
//}

static void drawAABB (aabb *aabb, render_command_list *renderCommands) {
    render_command_lines *lineCommand = startLines(renderCommands);
    vector3 min = aabb->min;
    vector3 max = aabb->max;
    drawLine(min.x, min.y, min.z, max.x, min.y, min.z, renderCommands, lineCommand);
    drawLine(max.x, min.y, min.z, max.x, min.y, max.z, renderCommands, lineCommand);
    drawLine(max.x, min.y, max.z, min.x, min.y, max.z, renderCommands, lineCommand);
    drawLine(min.x, min.y, max.z, min.x, min.y, min.z, renderCommands, lineCommand);
    drawLine(min.x, max.y, min.z, max.x, max.y, min.z, renderCommands, lineCommand);
    drawLine(max.x, max.y, min.z, max.x, max.y, max.z, renderCommands, lineCommand);
    drawLine(max.x, max.y, max.z, min.x, max.y, max.z, renderCommands, lineCommand);
    drawLine(min.x, max.y, max.z, min.x, max.y, min.z, renderCommands, lineCommand);
    drawLine(min.x, min.y, min.z, min.x, max.y, min.z, renderCommands, lineCommand);
    drawLine(max.x, min.y, min.z, max.x, max.y, min.z, renderCommands, lineCommand);
    drawLine(max.x, min.y, max.z, max.x, max.y, max.z, renderCommands, lineCommand);
    drawLine(min.x, min.y, max.z, min.x, max.y, max.z, renderCommands, lineCommand);
}

#endif
