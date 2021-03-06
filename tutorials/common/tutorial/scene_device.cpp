// ======================================================================== //
// Copyright 2009-2017 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "scene_device.h"

#define FIXED_EDGE_TESSELLATION_VALUE 4

namespace embree
{
  extern "C" int g_instancing_mode = 0;

  ISPCTriangleMesh::ISPCTriangleMesh (TutorialScene* scene_in, Ref<SceneGraph::TriangleMeshNode> in) 
    : geom(TRIANGLE_MESH)
  {
    positions = new Vec3fa*[in->numTimeSteps()];
    for (size_t i=0; i<in->numTimeSteps(); i++)
      positions[i] = in->positions[i].data();
    normals = in->normals.data();
    texcoords = in->texcoords.data();
    triangles = (ISPCTriangle*) in->triangles.data();
    numTimeSteps = (unsigned) in->numTimeSteps();
    numVertices = (unsigned) in->numVertices();
    numTriangles = (unsigned) in->numPrimitives();
    scene = nullptr;
    geomID = -1;
    materialID = scene_in->materialID(in->material);
  }

  ISPCTriangleMesh::~ISPCTriangleMesh () {
    delete[] positions;
  }
  
  ISPCQuadMesh::ISPCQuadMesh (TutorialScene* scene_in, Ref<SceneGraph::QuadMeshNode> in) 
    : geom(QUAD_MESH)
  {
    positions = new Vec3fa*[in->numTimeSteps()];
    for (size_t i=0; i<in->numTimeSteps(); i++)
      positions[i] = in->positions[i].data();
    normals = in->normals.data();
    texcoords = in->texcoords.data();
    quads = (ISPCQuad*) in->quads.data();
    numTimeSteps = (unsigned) in->numTimeSteps();
    numVertices = (unsigned) in->numVertices();
    numQuads = (unsigned) in->numPrimitives();
    scene = nullptr;
    geomID = -1;
    materialID = scene_in->materialID(in->material);
  }

  ISPCQuadMesh::~ISPCQuadMesh () {
    delete[] positions;
  }

  ISPCSubdivMesh::ISPCSubdivMesh (TutorialScene* scene_in, Ref<SceneGraph::SubdivMeshNode> in) 
    : geom(SUBDIV_MESH)
  {
    positions = new Vec3fa*[in->numTimeSteps()];
    for (size_t i=0; i<in->numTimeSteps(); i++)
      positions[i] = in->positions[i].data();
    normals = in->normals.data();
    texcoords = in->texcoords.data();
    position_indices = in->position_indices.data();
    normal_indices = in->normal_indices.data();
    texcoord_indices = in->texcoord_indices.data();
    position_subdiv_mode =  in->position_subdiv_mode;
    normal_subdiv_mode = in->normal_subdiv_mode;
    texcoord_subdiv_mode = in->texcoord_subdiv_mode;
    verticesPerFace = in->verticesPerFace.data();
    holes = in->holes.data();
    edge_creases = in->edge_creases.data();
    edge_crease_weights = in->edge_crease_weights.data();
    vertex_creases = in->vertex_creases.data();
    vertex_crease_weights = in->vertex_crease_weights.data();
    numTimeSteps = unsigned(in->numTimeSteps());
    numVertices = unsigned(in->numPositions());
    numFaces = unsigned(in->numPrimitives());
    numEdges = unsigned(in->position_indices.size());
    numEdgeCreases = unsigned(in->edge_creases.size());
    numVertexCreases = unsigned(in->vertex_creases.size());
    numHoles = unsigned(in->holes.size());
    numNormals = unsigned(in->normals.size());
    numTexCoords = unsigned(in->texcoords.size());
    materialID = scene_in->materialID(in->material);
    scene = nullptr;
    geomID = -1;
    
    size_t numEdges = in->position_indices.size();
    size_t numFaces = in->verticesPerFace.size();
    subdivlevel = new float[numEdges];
    face_offsets = new unsigned[numFaces];
    for (size_t i=0; i<numEdges; i++) subdivlevel[i] = 1.0f;
    int offset = 0;
    for (size_t i=0; i<numFaces; i++)
    {
      face_offsets[i] = offset;
      offset+=verticesPerFace[i];
    }
  }
  
  ISPCSubdivMesh::~ISPCSubdivMesh ()
  {
    delete[] positions;
    delete[] subdivlevel;
    delete[] face_offsets;
  }
  
  ISPCLineSegments::ISPCLineSegments (TutorialScene* scene_in, Ref<SceneGraph::LineSegmentsNode> in) 
    : geom(LINE_SEGMENTS)
  {
    positions = new Vec3fa*[in->numTimeSteps()];
    for (size_t i=0; i<in->numTimeSteps(); i++)
      positions[i] = in->positions[i].data();
    indices = in->indices.data();
    numTimeSteps = (unsigned) in->numTimeSteps();
    numVertices = (unsigned) in->numVertices();
    numSegments = (unsigned) in->numPrimitives();
    materialID = scene_in->materialID(in->material);
    scene = nullptr;
    geomID = -1;
  }

  ISPCLineSegments::~ISPCLineSegments () {
    delete[] positions;
  }
  
  ISPCHairSet::ISPCHairSet (TutorialScene* scene_in, SceneGraph::HairSetNode::Type type, SceneGraph::HairSetNode::Basis basis, Ref<SceneGraph::HairSetNode> in) 
    : geom(type == SceneGraph::HairSetNode::HAIR ? HAIR_SET : CURVES), basis(basis == SceneGraph::HairSetNode::BEZIER ? BEZIER_BASIS : BSPLINE_BASIS)
  {
    positions = new Vec3fa*[in->numTimeSteps()];
    for (size_t i=0; i<in->numTimeSteps(); i++)
      positions[i] = in->positions[i].data();
    hairs = (ISPCHair*) in->hairs.data();
    numTimeSteps = (unsigned) in->numTimeSteps();
    numVertices = (unsigned) in->numVertices();
    numHairs = (unsigned)in->numPrimitives();
    materialID = scene_in->materialID(in->material);
    scene = nullptr;
    geomID = -1;
    tessellation_rate = in->tessellation_rate;
  }

  ISPCHairSet::~ISPCHairSet() {
    delete[] positions;
  }

  ISPCInstance* ISPCInstance::create (TutorialScene* scene, Ref<SceneGraph::TransformNode> in) {
    return ::new (alignedMalloc(sizeof(ISPCInstance)+(in->spaces.size()-1)*sizeof(AffineSpace3fa))) ISPCInstance(scene,in);
  }
  
  ISPCInstance::ISPCInstance (TutorialScene* scene, Ref<SceneGraph::TransformNode> in)
    : geom(INSTANCE), geomID(scene->geometryID(in->child)), numTimeSteps(unsigned(in->spaces.size())) 
  {
    for (size_t i=0; i<numTimeSteps; i++)
      spaces[i] = in->spaces[i];
  }

  ISPCGroup::ISPCGroup (TutorialScene* scene, Ref<SceneGraph::GroupNode> in)
    : geom(GROUP)
  {
    numGeometries = in->size();
    geometries = new ISPCGeometry*[numGeometries];
    for (size_t i=0; i<numGeometries; i++)
      geometries[i] = ISPCScene::convertGeometry(scene,in->child(i));
  }
  
  ISPCGroup::~ISPCGroup()
  {
    for (size_t i=0; i<numGeometries; i++)
      delete geometries[i];
    delete[] geometries;
  }

  ISPCGeometry* ISPCScene::convertGeometry (TutorialScene* scene, Ref<SceneGraph::Node> in)
  {
    if (Ref<SceneGraph::TriangleMeshNode> mesh = in.dynamicCast<SceneGraph::TriangleMeshNode>())
      return (ISPCGeometry*) new ISPCTriangleMesh(scene,mesh);
    else if (Ref<SceneGraph::QuadMeshNode> mesh = in.dynamicCast<SceneGraph::QuadMeshNode>())
      return (ISPCGeometry*) new ISPCQuadMesh(scene,mesh);
    else if (Ref<SceneGraph::SubdivMeshNode> mesh = in.dynamicCast<SceneGraph::SubdivMeshNode>())
      return (ISPCGeometry*) new ISPCSubdivMesh(scene,mesh);
    else if (Ref<SceneGraph::LineSegmentsNode> mesh = in.dynamicCast<SceneGraph::LineSegmentsNode>())
      return (ISPCGeometry*) new ISPCLineSegments(scene,mesh);
    else if (Ref<SceneGraph::HairSetNode> mesh = in.dynamicCast<SceneGraph::HairSetNode>())
      return (ISPCGeometry*) new ISPCHairSet(scene,mesh->type,mesh->basis,mesh);
    else if (Ref<SceneGraph::TransformNode> mesh = in.dynamicCast<SceneGraph::TransformNode>())
      return (ISPCGeometry*) ISPCInstance::create(scene,mesh);
    else if (Ref<SceneGraph::GroupNode> mesh = in.dynamicCast<SceneGraph::GroupNode>())
      return (ISPCGeometry*) new ISPCGroup(scene,mesh);
    else
      THROW_RUNTIME_ERROR("unknown geometry type");
  }

  unsigned int ConvertTriangleMesh(ISPCTriangleMesh* mesh, RTCGeometryFlags gflags, RTCScene scene_out)
  {
    unsigned int geomID = rtcNewTriangleMesh (scene_out, gflags, mesh->numTriangles, mesh->numVertices, mesh->numTimeSteps);
    for (size_t t=0; t<mesh->numTimeSteps; t++) {
      rtcSetBuffer(scene_out, geomID, (RTCBufferType)(RTC_VERTEX_BUFFER+t), mesh->positions[t], 0, sizeof(Vec3fa      ));
    }
    rtcSetBuffer(scene_out, geomID, RTC_INDEX_BUFFER,  mesh->triangles, 0, sizeof(ISPCTriangle));
    mesh->scene = scene_out;
    mesh->geomID = geomID;
    return geomID;
  }
  
  unsigned int ConvertQuadMesh(ISPCQuadMesh* mesh, RTCGeometryFlags gflags, RTCScene scene_out)
  {
    unsigned int geomID = rtcNewQuadMesh (scene_out, gflags, mesh->numQuads, mesh->numVertices, mesh->numTimeSteps);
    for (size_t t=0; t<mesh->numTimeSteps; t++) {
      rtcSetBuffer(scene_out, geomID, (RTCBufferType)(RTC_VERTEX_BUFFER+t), mesh->positions[t], 0, sizeof(Vec3fa      ));
    }
    rtcSetBuffer(scene_out, geomID, RTC_INDEX_BUFFER,  mesh->quads, 0, sizeof(ISPCQuad));
    mesh->scene = scene_out;
    mesh->geomID = geomID;
    return geomID;
  }
  
  unsigned int ConvertSubdivMesh(ISPCSubdivMesh* mesh, RTCGeometryFlags gflags, RTCScene scene_out)
  {
    unsigned int geomID = rtcNewSubdivisionMesh(scene_out, gflags, mesh->numFaces, mesh->numEdges, mesh->numVertices,
                                                mesh->numEdgeCreases, mesh->numVertexCreases, mesh->numHoles, mesh->numTimeSteps);
    for (size_t i=0; i<mesh->numEdges; i++) mesh->subdivlevel[i] = FIXED_EDGE_TESSELLATION_VALUE;
    for (size_t t=0; t<mesh->numTimeSteps; t++) {
      rtcSetBuffer(scene_out, geomID, (RTCBufferType)(RTC_VERTEX_BUFFER+t), mesh->positions[t], 0, sizeof(Vec3fa  ));
    }
    rtcSetBuffer(scene_out, geomID, RTC_LEVEL_BUFFER,  mesh->subdivlevel, 0, sizeof(float));

    /* create geometry topology */
    rtcSetBuffer(scene_out, geomID, RTC_INDEX_BUFFER,  mesh->position_indices  , 0, sizeof(unsigned int));
    rtcSetSubdivisionMode(scene_out, geomID, 0, mesh->position_subdiv_mode);

    /* set normal buffers and optionally normal topology */
    if (mesh->normals) {
      rtcSetBuffer2(scene_out, geomID, (RTCBufferType)(RTC_USER_VERTEX_BUFFER+1), mesh->normals, 0, sizeof(Vec3fa  ), mesh->numNormals);
      if (mesh->normal_indices) {
        rtcSetBuffer(scene_out, geomID, (RTCBufferType)(RTC_INDEX_BUFFER+1),  mesh->normal_indices  , 0, sizeof(unsigned int));
        rtcSetIndexBuffer(scene_out, geomID, (RTCBufferType)(RTC_USER_VERTEX_BUFFER+1), (RTCBufferType)(RTC_INDEX_BUFFER+1));
        rtcSetSubdivisionMode(scene_out, geomID, 1, mesh->normal_subdiv_mode);
      }
    }

    /* set texcoord buffer and optionally texcoord topology */
    if (mesh->texcoords) {
      rtcSetBuffer2(scene_out, geomID, (RTCBufferType)(RTC_USER_VERTEX_BUFFER+2), mesh->texcoords, 0, sizeof(Vec2f), mesh->numTexCoords);
      if (mesh->texcoord_indices) {
        rtcSetBuffer(scene_out, geomID, (RTCBufferType)(RTC_INDEX_BUFFER+2),  mesh->texcoord_indices  , 0, sizeof(unsigned int));
        rtcSetIndexBuffer(scene_out, geomID, (RTCBufferType)(RTC_USER_VERTEX_BUFFER+2), (RTCBufferType)(RTC_INDEX_BUFFER+2));
        rtcSetSubdivisionMode(scene_out, geomID, 2, mesh->texcoord_subdiv_mode);
      }
    }

    rtcSetBuffer(scene_out, geomID, RTC_FACE_BUFFER,   mesh->verticesPerFace, 0, sizeof(unsigned int));
    rtcSetBuffer(scene_out, geomID, RTC_HOLE_BUFFER,   mesh->holes, 0, sizeof(unsigned int));
    rtcSetBuffer(scene_out, geomID, RTC_EDGE_CREASE_INDEX_BUFFER,    mesh->edge_creases,          0, 2*sizeof(unsigned int));
    rtcSetBuffer(scene_out, geomID, RTC_EDGE_CREASE_WEIGHT_BUFFER,   mesh->edge_crease_weights,   0, sizeof(float));
    rtcSetBuffer(scene_out, geomID, RTC_VERTEX_CREASE_INDEX_BUFFER,  mesh->vertex_creases,        0, sizeof(unsigned int));
    rtcSetBuffer(scene_out, geomID, RTC_VERTEX_CREASE_WEIGHT_BUFFER, mesh->vertex_crease_weights, 0, sizeof(float));
    mesh->scene = scene_out;
    mesh->geomID = geomID;
    return geomID;
  }
  
  unsigned int ConvertLineSegments(ISPCLineSegments* mesh, RTCGeometryFlags gflags, RTCScene scene_out)
  {
    unsigned int geomID = rtcNewLineSegments (scene_out, gflags, mesh->numSegments, mesh->numVertices, mesh->numTimeSteps);
    for (size_t t=0; t<mesh->numTimeSteps; t++) {
      rtcSetBuffer(scene_out,geomID,(RTCBufferType)(RTC_VERTEX_BUFFER+t), mesh->positions[t],0,sizeof(Vec3fa));
    }
    rtcSetBuffer(scene_out,geomID,RTC_INDEX_BUFFER,mesh->indices,0,sizeof(int));
    mesh->scene = scene_out;
    mesh->geomID = geomID;
    return geomID;
  }
  
  unsigned int ConvertHairSet(ISPCHairSet* mesh, RTCGeometryFlags gflags, RTCScene scene_out)
  {
    unsigned int geomID = mesh->basis == BEZIER_BASIS ?
      rtcNewBezierHairGeometry  (scene_out, gflags, mesh->numHairs, mesh->numVertices, mesh->numTimeSteps) :
      rtcNewBSplineHairGeometry (scene_out, gflags, mesh->numHairs, mesh->numVertices, mesh->numTimeSteps);

    for (size_t t=0; t<mesh->numTimeSteps; t++) {
      rtcSetBuffer(scene_out,geomID,(RTCBufferType)(RTC_VERTEX_BUFFER+t), mesh->positions[t],0,sizeof(Vec3fa));
    }
    rtcSetBuffer(scene_out,geomID,RTC_INDEX_BUFFER,mesh->hairs,0,sizeof(ISPCHair));
    rtcSetTessellationRate(scene_out,geomID,(float)mesh->tessellation_rate);
    mesh->scene = scene_out;
    mesh->geomID = geomID;
    return geomID;
  }
  
  unsigned int ConvertCurveGeometry(ISPCHairSet* mesh, RTCGeometryFlags gflags, RTCScene scene_out)
  {
    unsigned int geomID = mesh->basis == BEZIER_BASIS ?
      rtcNewBezierCurveGeometry  (scene_out, gflags, mesh->numHairs, mesh->numVertices, mesh->numTimeSteps) :
      rtcNewBSplineCurveGeometry (scene_out, gflags, mesh->numHairs, mesh->numVertices, mesh->numTimeSteps);

    for (size_t t=0; t<mesh->numTimeSteps; t++) {
      rtcSetBuffer(scene_out,geomID,(RTCBufferType)(RTC_VERTEX_BUFFER+t), mesh->positions[t],0,sizeof(Vec3fa));
    }
    rtcSetBuffer(scene_out,geomID,RTC_INDEX_BUFFER,mesh->hairs,0,sizeof(ISPCHair));
    mesh->scene = scene_out;
    mesh->geomID = geomID;
    return geomID;
  }
  
  void ConvertGroup(ISPCGroup* group, RTCGeometryFlags gflags, RTCScene scene_out)
  {
    for (size_t i=0; i<group->numGeometries; i++)
    {
      ISPCGeometry* geometry = group->geometries[i];
      if (geometry->type == SUBDIV_MESH)
        ConvertSubdivMesh((ISPCSubdivMesh*) geometry, gflags, scene_out);
      else if (geometry->type == TRIANGLE_MESH)
        ConvertTriangleMesh((ISPCTriangleMesh*) geometry, gflags, scene_out);
      else if (geometry->type == QUAD_MESH)
        ConvertQuadMesh((ISPCQuadMesh*) geometry, gflags, scene_out);
      else if (geometry->type == LINE_SEGMENTS)
        ConvertLineSegments((ISPCLineSegments*) geometry, gflags, scene_out);
      else if (geometry->type == HAIR_SET)
        ConvertHairSet((ISPCHairSet*) geometry, gflags, scene_out);
      else if (geometry->type == CURVES)
        ConvertCurveGeometry((ISPCHairSet*) geometry, gflags, scene_out);
      else
        assert(false);
    }
  }
  
  unsigned int ConvertInstance(ISPCScene* scene_in, ISPCInstance* instance, int meshID, RTCScene scene_out)
  {
    if (g_instancing_mode == 1) {
      throw std::runtime_error("geometry instances not supported yet");
    } 
    else
    {
      RTCScene scene_inst = scene_in->geomID_to_scene[instance->geomID];
      if (instance->numTimeSteps == 1) {
        unsigned int geomID = rtcNewInstance2(scene_out, scene_inst, 1);
        rtcSetTransform2(scene_out,geomID,RTC_MATRIX_COLUMN_MAJOR_ALIGNED16,&instance->spaces[0].l.vx.x,0);
        return geomID;
      }
      else {
        unsigned int geomID = rtcNewInstance2(scene_out, scene_inst, instance->numTimeSteps);
        for (size_t t=0; t<instance->numTimeSteps; t++)
          rtcSetTransform2(scene_out,geomID,RTC_MATRIX_COLUMN_MAJOR_ALIGNED16,&instance->spaces[t].l.vx.x,t);
        return geomID;
      }
    }
  }
  
  typedef ISPCInstance* ISPCInstance_ptr;
  typedef ISPCGeometry* ISPCGeometry_ptr;
  
  extern "C" RTCScene ConvertScene(RTCDevice g_device, ISPCScene* scene_in, RTCSceneFlags sflags, RTCAlgorithmFlags aflags, RTCGeometryFlags gflags)
  {
    RTCScene scene_out = rtcDeviceNewScene(g_device,sflags,aflags);
    
    /* use geometry instancing feature */
    if (g_instancing_mode == 1)
    {
      for (unsigned int i=0; i<scene_in->numGeometries; i++)
      {
        ISPCGeometry* geometry = scene_in->geometries[i];
        if (geometry->type == SUBDIV_MESH) {
          unsigned int geomID = ConvertSubdivMesh((ISPCSubdivMesh*) geometry, gflags, scene_out);
          assert(geomID == i);
          rtcDisable(scene_out,geomID);
        }
        else if (geometry->type == TRIANGLE_MESH) {
          unsigned int geomID = ConvertTriangleMesh((ISPCTriangleMesh*) geometry, gflags, scene_out);
          assert(geomID == i);
          rtcDisable(scene_out,geomID);
        }
        else if (geometry->type == QUAD_MESH) {
          unsigned int geomID = ConvertQuadMesh((ISPCQuadMesh*) geometry, gflags, scene_out);
          assert(geomID == i);
          rtcDisable(scene_out,geomID);
        }
        else if (geometry->type == LINE_SEGMENTS) {
          unsigned int geomID = ConvertLineSegments((ISPCLineSegments*) geometry, gflags, scene_out);
          assert(geomID == i);
          rtcDisable(scene_out,geomID);
        }
        else if (geometry->type == HAIR_SET) {
          unsigned int geomID = ConvertHairSet((ISPCHairSet*) geometry, gflags, scene_out);
          assert(geomID == i);
          rtcDisable(scene_out,geomID);
        }
        else if (geometry->type == CURVES) {
          unsigned int geomID = ConvertCurveGeometry((ISPCHairSet*) geometry, gflags, scene_out);
          assert(geomID == i);
          rtcDisable(scene_out,geomID);
        }
        else if (geometry->type == INSTANCE) {
          unsigned int geomID = ConvertInstance(scene_in, (ISPCInstance*) geometry, i, scene_out);
          assert(geomID == i); scene_in->geomID_to_inst[geomID] = (ISPCInstance*) geometry;
        }
        else
          assert(false);
      }
    }
    
    /* use scene instancing feature */
    else if (g_instancing_mode == 2 || g_instancing_mode == 3)
    {
      for (unsigned int i=0; i<scene_in->numGeometries; i++)
      {
        ISPCGeometry* geometry = scene_in->geometries[i];
        if (geometry->type == SUBDIV_MESH) {
          RTCScene objscene = rtcDeviceNewScene(g_device,sflags,aflags);
          ConvertSubdivMesh((ISPCSubdivMesh*) geometry,gflags,objscene);
          scene_in->geomID_to_scene[i] = objscene;
          //rtcCommit(objscene);
        }
        else if (geometry->type == TRIANGLE_MESH) {
          RTCScene objscene = rtcDeviceNewScene(g_device,sflags,aflags);
          ConvertTriangleMesh((ISPCTriangleMesh*) geometry,gflags,objscene);
          scene_in->geomID_to_scene[i] = objscene;
          //rtcCommit(objscene);
        }
        else if (geometry->type == QUAD_MESH) {
          RTCScene objscene = rtcDeviceNewScene(g_device,sflags,aflags);
          ConvertQuadMesh((ISPCQuadMesh*) geometry,gflags,objscene);
          scene_in->geomID_to_scene[i] = objscene;
          //rtcCommit(objscene);
        }
        else if (geometry->type == LINE_SEGMENTS) {
          RTCScene objscene = rtcDeviceNewScene(g_device,sflags,aflags);
          ConvertLineSegments((ISPCLineSegments*) geometry,gflags,objscene);
          scene_in->geomID_to_scene[i] = objscene;
          //rtcCommit(objscene);
        }
        else if (geometry->type == HAIR_SET) {
          RTCScene objscene = rtcDeviceNewScene(g_device,sflags,aflags);
          ConvertHairSet((ISPCHairSet*) geometry,gflags,objscene);
          scene_in->geomID_to_scene[i] = objscene;
          //rtcCommit(objscene);
        }
        else if (geometry->type == CURVES) {
          RTCScene objscene = rtcDeviceNewScene(g_device,sflags,aflags);
          ConvertCurveGeometry((ISPCHairSet*) geometry,gflags,objscene);
          scene_in->geomID_to_scene[i] = objscene;
          //rtcCommit(objscene);
        }
        else if (geometry->type == GROUP) {
          RTCScene objscene = rtcDeviceNewScene(g_device,sflags,aflags);
          ConvertGroup((ISPCGroup*) geometry,gflags,objscene);
          scene_in->geomID_to_scene[i] = objscene;
          //rtcCommit(objscene);
        }
        else if (geometry->type == INSTANCE) {
          unsigned int geomID = ConvertInstance(scene_in, (ISPCInstance*) geometry, i, scene_out);
          scene_in->geomID_to_scene[i] = nullptr; scene_in->geomID_to_inst[geomID] = (ISPCInstance*) geometry;
        }
        else
          assert(false);
      }
    }
    
    /* no instancing */
    else
    {
      for (unsigned int i=0; i<scene_in->numGeometries; i++)
      {
        ISPCGeometry* geometry = scene_in->geometries[i];
        if (geometry->type == SUBDIV_MESH) {
          unsigned int geomID MAYBE_UNUSED = ConvertSubdivMesh((ISPCSubdivMesh*) geometry, gflags, scene_out);
          assert(geomID == i);
        }
        else if (geometry->type == TRIANGLE_MESH) {
          unsigned int geomID MAYBE_UNUSED = ConvertTriangleMesh((ISPCTriangleMesh*) geometry, gflags, scene_out);
          assert(geomID == i);
        }
        else if (geometry->type == QUAD_MESH) {
          unsigned int geomID MAYBE_UNUSED = ConvertQuadMesh((ISPCQuadMesh*) geometry, gflags, scene_out);
          assert(geomID == i);
        }
        else if (geometry->type == LINE_SEGMENTS) {
          unsigned int geomID MAYBE_UNUSED = ConvertLineSegments((ISPCLineSegments*) geometry, gflags, scene_out);
          assert(geomID == i);
        }
        else if (geometry->type == HAIR_SET) {
          unsigned int geomID MAYBE_UNUSED = ConvertHairSet((ISPCHairSet*) geometry, gflags, scene_out);
          assert(geomID == i);
        }
        else if (geometry->type == CURVES) {
          unsigned int geomID MAYBE_UNUSED = ConvertCurveGeometry((ISPCHairSet*) geometry, gflags, scene_out);
          assert(geomID == i);
        }
        else
          assert(false);
      }
    }
    return scene_out;
  }
}
