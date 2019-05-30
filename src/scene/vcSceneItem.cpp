#include "vcSceneItem.h"

#include "vcState.h"
#include "vcRender.h"

vdkProjectNode* vcSceneItem_CreateNodeInProject(vdkProject *pProject, const char *pType, const char *pName)
{
  vdkProjectNode *pNode = nullptr;

  vdkProjectNode_Create(pProject, &pNode, pType, pName, nullptr, nullptr);

  UDASSERT(pNode != nullptr, "Remove Path to this- memory alloc failed.");

  return pNode;
}

vcSceneItem::vcSceneItem(vdkProjectNode *pNode, vcState * /*pProgramState*/) :
  m_pProject(nullptr),
  m_loadStatus(0),
  m_visible(true),
  m_selected(false),
  m_expanded(false),
  m_editName(false),
  m_moved(false),
  m_pPreferredProjection(nullptr),
  m_pCurrentProjection(nullptr)
{
  m_metadata.SetVoid();
  m_pNode = pNode;
}

vcSceneItem::vcSceneItem(vcState *pProgramState, const char *pType, const char *pName) :
  vcSceneItem(vcSceneItem_CreateNodeInProject(pProgramState->sceneExplorer.pProject, pType, pName), pProgramState)
{
  // Do nothing
}

vcSceneItem::~vcSceneItem()
{
  m_metadata.Destroy();
  udFree(m_pPreferredProjection);
  udFree(m_pCurrentProjection);
}

void vcSceneItem::ChangeProjection(const udGeoZone &newZone)
{
  if (m_pCurrentProjection == nullptr)
  {
    udDouble3 itemPos = udGeoZone_ToLatLong(newZone, GetWorldSpacePivot());
    // If min == max then there are no bounds
    if (newZone.latLongBoundMin == newZone.latLongBoundMax || (itemPos.x <= newZone.latLongBoundMax.x && itemPos.x >= newZone.latLongBoundMin.x && itemPos.y <= newZone.latLongBoundMax.y && itemPos.y >= newZone.latLongBoundMin.y))
    {
      m_pPreferredProjection = udAllocType(udGeoZone, 1, udAF_Zero);
      memcpy(m_pPreferredProjection, &newZone, sizeof(udGeoZone));
      m_pCurrentProjection = udAllocType(udGeoZone, 1, udAF_Zero);
      memcpy(m_pCurrentProjection, &newZone, sizeof(udGeoZone));
    }
  }
  else if (newZone.srid != m_pCurrentProjection->srid)
  {
    memcpy(m_pCurrentProjection, &newZone, sizeof(newZone));
  }
}

void vcSceneItem::SetCameraPosition(vcState *pProgramState)
{
  pProgramState->pCamera->position = GetWorldSpacePivot();
}

udDouble3 vcSceneItem::GetWorldSpacePivot()
{
  return (this->GetWorldSpaceMatrix() * udDouble4::create(this->GetLocalSpacePivot(), 1.0)).toVector3();
}

udDouble3 vcSceneItem::GetLocalSpacePivot()
{
  return udDouble3::zero(); //TODO: Consider somehow defaulting to Camera Position instead
}

udDouble4x4 vcSceneItem::GetWorldSpaceMatrix()
{
  return udDouble4x4::identity();
}

void vcSceneItem::UpdateNode()
{
  this->OnNodeUpdate();
  m_lastUpdateTime = m_pNode->lastUpdate;
};
