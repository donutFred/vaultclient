#include "vcSettingsUI.h"

#include "vcState.h"
#include "vcSettings.h"
#include "vcRender.h"
#include "vcModals.h"
#include "vcClassificationColours.h"

#include "imgui_ex/vcImGuiSimpleWidgets.h"

#include "udStringUtil.h"

void vcSettingsUI_Show(vcState *pProgramState)
{
  if (ImGui::Begin(udTempStr("%s###settingsDock", vcString::Get("settingsTitle")), &pProgramState->settings.window.windowsOpen[vcDocks_Settings]))
  {
    bool opened = ImGui::CollapsingHeader(vcString::Get("AppearanceID"));

    if (ImGui::BeginPopupContextItem("AppearanceContext"))
    {
      if (ImGui::Selectable(vcString::Get("AppearanceRestore")))
      {
        vcSettings_Load(&pProgramState->settings, true, vcSC_Appearance);
      }
      ImGui::EndPopup();
    }

    if (opened)
    {
      int styleIndex = pProgramState->settings.presentation.styleIndex - 1;

      const char *themeOptions[] = { vcString::Get("settingsAppearanceDark"), vcString::Get("settingsAppearanceLight") };
      if (ImGui::Combo(vcString::Get("settingsAppearanceTheme"), &styleIndex, themeOptions, (int)udLengthOf(themeOptions)))
      {
        pProgramState->settings.presentation.styleIndex = styleIndex + 1;
        switch (styleIndex)
        {
        case 0: ImGui::StyleColorsDark(); break;
        case 1: ImGui::StyleColorsLight(); break;
        }
      }

      // Checks so the casts below are safe
      UDCOMPILEASSERT(sizeof(pProgramState->settings.presentation.mouseAnchor) == sizeof(int), "MouseAnchor is no longer sizeof(int)");

      if (ImGui::SliderFloat(vcString::Get("settingsAppearancePOIDistance"), &pProgramState->settings.presentation.POIFadeDistance, vcSL_POIFaderMin, vcSL_POIFaderMax, "%.3fm", 3.f))
        pProgramState->settings.presentation.POIFadeDistance = udClamp(pProgramState->settings.presentation.POIFadeDistance, vcSL_POIFaderMin, vcSL_GlobalLimitf);
      ImGui::Checkbox(vcString::Get("settingsAppearanceShowDiagnostics"), &pProgramState->settings.presentation.showDiagnosticInfo);
      ImGui::Checkbox(vcString::Get("settingsAppearanceAdvancedGIS"), &pProgramState->settings.presentation.showAdvancedGIS);
      ImGui::Checkbox(vcString::Get("settingsAppearanceLimitFPS"), &pProgramState->settings.presentation.limitFPSInBackground);
      ImGui::Checkbox(vcString::Get("settingsAppearanceShowCompass"), &pProgramState->settings.presentation.showCompass);

      const char *presentationOptions[] = { vcString::Get("settingsAppearanceHide"), vcString::Get("settingsAppearanceShow"), vcString::Get("settingsAppearanceResponsive") };
      if (ImGui::Combo(vcString::Get("settingsAppearancePresentationUI"), (int*)&pProgramState->settings.responsiveUI, presentationOptions, (int)udLengthOf(presentationOptions)))
        pProgramState->showUI = false;

      const char *anchorOptions[] = { vcString::Get("settingsAppearanceNone"), vcString::Get("settingsAppearanceOrbit"), vcString::Get("settingsAppearanceCompass") };
      ImGui::Combo(vcString::Get("settingsAppearanceMouseAnchor"), (int*)&pProgramState->settings.presentation.mouseAnchor, anchorOptions, (int)udLengthOf(anchorOptions));

      const char *voxelOptions[] = { vcString::Get("settingsAppearanceRectangles"), vcString::Get("settingsAppearanceCubes"), vcString::Get("settingsAppearancePoints") };
      ImGui::Combo(vcString::Get("settingsAppearanceVoxelShape"), &pProgramState->settings.presentation.pointMode, voxelOptions, (int)udLengthOf(voxelOptions));
    }

    bool opened2 = ImGui::CollapsingHeader(vcString::Get("InputControlsID"));
    if (ImGui::BeginPopupContextItem("InputContext"))
    {
      if (ImGui::Selectable(vcString::Get("InputRestore")))
      {
        vcSettings_Load(&pProgramState->settings, true, vcSC_InputControls);
      }
      ImGui::EndPopup();
    }
    if (opened2)
    {
      ImGui::Checkbox(vcString::Get("settingsControlsOSC"), &pProgramState->settings.onScreenControls);
      if (ImGui::Checkbox(vcString::Get("settingsControlsTouchUI"), &pProgramState->settings.window.touchscreenFriendly))
      {
        ImGuiStyle& style = ImGui::GetStyle();
        style.TouchExtraPadding = pProgramState->settings.window.touchscreenFriendly ? ImVec2(4, 4) : ImVec2();
      }

      ImGui::Checkbox(vcString::Get("settingsControlsInvertX"), &pProgramState->settings.camera.invertX);
      ImGui::Checkbox(vcString::Get("settingsControlsInvertY"), &pProgramState->settings.camera.invertY);

      ImGui::TextUnformatted(vcString::Get("settingsControlsMousePivot"));
      const char *mouseModes[] = { vcString::Get("settingsControlsTumble"), vcString::Get("settingsControlsOrbit"), vcString::Get("settingsControlsPan"), vcString::Get("settingsControlsForward") };
      const char *scrollwheelModes[] = { vcString::Get("settingsControlsDolly"), vcString::Get("settingsControlsChangeMoveSpeed") };

      // Checks so the casts below are safe
      UDCOMPILEASSERT(sizeof(pProgramState->settings.camera.cameraMouseBindings[0]) == sizeof(int), "Bindings is no longer sizeof(int)");
      UDCOMPILEASSERT(sizeof(pProgramState->settings.camera.scrollWheelMode) == sizeof(int), "ScrollWheel is no longer sizeof(int)");

      ImGui::Combo(vcString::Get("settingsControlsLeft"), (int*)&pProgramState->settings.camera.cameraMouseBindings[0], mouseModes, (int)udLengthOf(mouseModes));
      ImGui::Combo(vcString::Get("settingsControlsMiddle"), (int*)&pProgramState->settings.camera.cameraMouseBindings[2], mouseModes, (int)udLengthOf(mouseModes));
      ImGui::Combo(vcString::Get("settingsControlsRight"), (int*)&pProgramState->settings.camera.cameraMouseBindings[1], mouseModes, (int)udLengthOf(mouseModes));
      ImGui::Combo(vcString::Get("settingsControlsScrollWheel"), (int*)&pProgramState->settings.camera.scrollWheelMode, scrollwheelModes, (int)udLengthOf(scrollwheelModes));
    }

    bool opened3 = ImGui::CollapsingHeader(vcString::Get("ViewportID"));
    if (ImGui::BeginPopupContextItem("ViewportContext"))
    {
      if (ImGui::Selectable(vcString::Get("ViewportRestore")))
      {
        vcSettings_Load(&pProgramState->settings, true, vcSC_Viewport);
      }
      ImGui::EndPopup();
    }
    if (opened3)
    {
      if (ImGui::SliderFloat(vcString::Get("settingsViewportViewDistance"), &pProgramState->settings.camera.farPlane, vcSL_CameraFarPlaneMin, vcSL_CameraFarPlaneMax, "%.3fm", 2.f))
      {
        pProgramState->settings.camera.nearPlane = pProgramState->settings.camera.farPlane * vcSL_CameraFarToNearPlaneRatio;
      }

      //const char *pLensOptions = " Custom FoV\0 7mm\0 11mm\0 15mm\0 24mm\0 30mm\0 50mm\0 70mm\0 100mm\0";
      if (ImGui::Combo(vcString::Get("settingsViewportCameraLens"), &pProgramState->settings.camera.lensIndex, vcCamera_GetLensNames(), vcLS_TotalLenses))
      {
        switch (pProgramState->settings.camera.lensIndex)
        {
        case vcLS_Custom:
          /*Custom FoV*/
          break;
        case vcLS_15mm:
          pProgramState->settings.camera.fieldOfView = vcLens15mm;
          break;
        case vcLS_24mm:
          pProgramState->settings.camera.fieldOfView = vcLens24mm;
          break;
        case vcLS_30mm:
          pProgramState->settings.camera.fieldOfView = vcLens30mm;
          break;
        case vcLS_50mm:
          pProgramState->settings.camera.fieldOfView = vcLens50mm;
          break;
        case vcLS_70mm:
          pProgramState->settings.camera.fieldOfView = vcLens70mm;
          break;
        case vcLS_100mm:
          pProgramState->settings.camera.fieldOfView = vcLens100mm;
          break;
        }
      }

      if (pProgramState->settings.camera.lensIndex == vcLS_Custom)
      {
        float fovDeg = UD_RAD2DEGf(pProgramState->settings.camera.fieldOfView);
        if (ImGui::SliderFloat(vcString::Get("settingsViewportFOV"), &fovDeg, vcSL_CameraFieldOfViewMin, vcSL_CameraFieldOfViewMax, vcString::Get("DegreesFormat")))
          pProgramState->settings.camera.fieldOfView = UD_DEG2RADf(udClamp(fovDeg, vcSL_CameraFieldOfViewMin, vcSL_CameraFieldOfViewMax));
      }
    }

    bool opened4 = ImGui::CollapsingHeader(vcString::Get("ElevationFormat"));
    if (ImGui::BeginPopupContextItem("MapsContext"))
    {
      if (ImGui::Selectable(vcString::Get("MapsRestore")))
      {
        vcSettings_Load(&pProgramState->settings, true, vcSC_MapsElevation);
        vcRender_ClearTiles(pProgramState->pRenderContext); // refresh map tiles since they just got updated
      }
      ImGui::EndPopup();
    }
    if (opened4)
    {
      ImGui::Checkbox(vcString::Get("settingsMapsMapTiles"), &pProgramState->settings.maptiles.mapEnabled);

      if (pProgramState->settings.maptiles.mapEnabled)
      {
        ImGui::Checkbox(vcString::Get("settingsMapsMouseLock"), &pProgramState->settings.maptiles.mouseInteracts);

        if (ImGui::Button(vcString::Get("settingsMapsTileServerButton"), ImVec2(-1, 0)))
          vcModals_OpenModal(pProgramState, vcMT_TileServer);

        if (ImGui::SliderFloat(vcString::Get("settingsMapsMapHeight"), &pProgramState->settings.maptiles.mapHeight, vcSL_MapHeightMin, vcSL_MapHeightMax, "%.3fm", 2.f))
          pProgramState->settings.maptiles.mapHeight = udClamp(pProgramState->settings.maptiles.mapHeight, -vcSL_GlobalLimitf, vcSL_GlobalLimitf);

        const char* blendModes[] = { vcString::Get("settingsMapsHybrid"), vcString::Get("settingsMapsOverlay"), vcString::Get("settingsMapsUnderlay") };
        if (ImGui::BeginCombo(vcString::Get("settingsMapsBlending"), blendModes[pProgramState->settings.maptiles.blendMode]))
        {
          for (size_t n = 0; n < UDARRAYSIZE(blendModes); ++n)
          {
            bool isSelected = (pProgramState->settings.maptiles.blendMode == n);

            if (ImGui::Selectable(blendModes[n], isSelected))
              pProgramState->settings.maptiles.blendMode = (vcMapTileBlendMode)n;

            if (isSelected)
              ImGui::SetItemDefaultFocus();
          }

          ImGui::EndCombo();
        }

        if (ImGui::SliderFloat(vcString::Get("settingsMapsOpacity"), &pProgramState->settings.maptiles.transparency, vcSL_OpacityMin, vcSL_OpacityMax, "%.3f"))
          pProgramState->settings.maptiles.transparency = udClamp(pProgramState->settings.maptiles.transparency, vcSL_OpacityMin, vcSL_OpacityMax);

        if (ImGui::Button(vcString::Get("settingsMapsSetHeight")))
          pProgramState->settings.maptiles.mapHeight = (float)pProgramState->pCamera->position.z;
      }
    }

    bool opened5 = ImGui::CollapsingHeader(vcString::Get("VisualizationFormat"));
    if (ImGui::BeginPopupContextItem("VisualizationContext"))
    {
      if (ImGui::Selectable(vcString::Get("VisualizationRestore")))
      {
        vcSettings_Load(&pProgramState->settings, true, vcSC_Visualization);
      }
      ImGui::EndPopup();
    }
    if (opened5)
    {
      const char *visualizationModes[] = { vcString::Get("settingsVisModeColour"), vcString::Get("settingsVisModeIntensity"), vcString::Get("settingsVisModeClassification") };
      ImGui::Combo(vcString::Get("settingsVisDisplayMode"), (int*)&pProgramState->settings.visualization.mode, visualizationModes, (int)udLengthOf(visualizationModes));

      if (pProgramState->settings.visualization.mode == vcVM_Intensity)
      {
        vcIGSW_StickyIntSlider(vcString::Get("settingsVisMinIntensity"), &pProgramState->settings.visualization.minIntensity, (int)vcSL_IntensityMin, pProgramState->settings.visualization.maxIntensity, 255);
        vcIGSW_StickyIntSlider(vcString::Get("settingsVisMaxIntensity"), &pProgramState->settings.visualization.maxIntensity, pProgramState->settings.visualization.minIntensity, (int)vcSL_IntensityMax, 255);
      }

      if (pProgramState->settings.visualization.mode == vcVM_Classification)
      {
        ImGui::Checkbox(vcString::Get("settingsVisClassShowColourTable"), &pProgramState->settings.visualization.useCustomClassificationColours);

        if (pProgramState->settings.visualization.useCustomClassificationColours)
        {
          ImGui::SameLine();
          if (ImGui::Button(vcString::Get("RestoreColoursID")))
            memcpy(pProgramState->settings.visualization.customClassificationColors, GeoverseClassificationColours, sizeof(pProgramState->settings.visualization.customClassificationColors));

          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassNeverClassified"), &pProgramState->settings.visualization.customClassificationColors[0], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassUnclassified"), &pProgramState->settings.visualization.customClassificationColors[1], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassGround"), &pProgramState->settings.visualization.customClassificationColors[2], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassLowVegetation"), &pProgramState->settings.visualization.customClassificationColors[3], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassMediumVegetation"), &pProgramState->settings.visualization.customClassificationColors[4], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassHighVegetation"), &pProgramState->settings.visualization.customClassificationColors[5], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassBuilding"), &pProgramState->settings.visualization.customClassificationColors[6], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassLowPoint"), &pProgramState->settings.visualization.customClassificationColors[7], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassKeyPoint"), &pProgramState->settings.visualization.customClassificationColors[8], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassWater"), &pProgramState->settings.visualization.customClassificationColors[9], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassRail"), &pProgramState->settings.visualization.customClassificationColors[10], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassRoadSurface"), &pProgramState->settings.visualization.customClassificationColors[11], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassReserved"), &pProgramState->settings.visualization.customClassificationColors[12], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassWireGuard"), &pProgramState->settings.visualization.customClassificationColors[13], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassWireConductor"), &pProgramState->settings.visualization.customClassificationColors[14], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassTransmissionTower"), &pProgramState->settings.visualization.customClassificationColors[15], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassWireStructureConnector"), &pProgramState->settings.visualization.customClassificationColors[16], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassBridgeDeck"), &pProgramState->settings.visualization.customClassificationColors[17], ImGuiColorEditFlags_NoAlpha);
          vcIGSW_ColorPickerU32(vcString::Get("settingsVisClassHighNoise"), &pProgramState->settings.visualization.customClassificationColors[18], ImGuiColorEditFlags_NoAlpha);

          if (ImGui::TreeNode(vcString::Get("settingsVisClassReservedColours")))
          {
            for (int i = 19; i < 64; ++i)
              vcIGSW_ColorPickerU32(udTempStr("%d. %s", i, vcString::Get("settingsVisClassReservedLabels")), &pProgramState->settings.visualization.customClassificationColors[i], ImGuiColorEditFlags_NoAlpha);
            ImGui::TreePop();
          }

          if (ImGui::TreeNode(vcString::Get("settingsVisClassUserDefinable")))
          {
            for (int i = 64; i <= 255; ++i)
            {
              char buttonID[12], inputID[3];
              if (pProgramState->settings.visualization.customClassificationColorLabels[i] == nullptr)
                vcIGSW_ColorPickerU32(udTempStr("%d. %s", i, vcString::Get("settingsVisClassUserDefined")), &pProgramState->settings.visualization.customClassificationColors[i], ImGuiColorEditFlags_NoAlpha);
              else
                vcIGSW_ColorPickerU32(udTempStr("%d. %s", i, pProgramState->settings.visualization.customClassificationColorLabels[i]), &pProgramState->settings.visualization.customClassificationColors[i], ImGuiColorEditFlags_NoAlpha);
              udSprintf(buttonID, 12, "%s##%d", vcString::Get("settingsVisClassRename"), i);
              udSprintf(inputID, 3, "##I%d", i);
              ImGui::SameLine();
              if (ImGui::Button(buttonID))
              {
                pProgramState->renaming = i;
                pProgramState->renameText[0] = '\0';
              }
              if (pProgramState->renaming == i)
              {
                ImGui::InputText(inputID, pProgramState->renameText, 30, ImGuiInputTextFlags_AutoSelectAll);
                ImGui::SameLine();
                if (ImGui::Button(vcString::Get("settingsVisClassSet")))
                {
                  if (pProgramState->settings.visualization.customClassificationColorLabels[i] != nullptr)
                    udFree(pProgramState->settings.visualization.customClassificationColorLabels[i]);
                  pProgramState->settings.visualization.customClassificationColorLabels[i] = udStrdup(pProgramState->renameText);
                  pProgramState->renaming = -1;
                }
              }
            }
            ImGui::TreePop();
          }
        }
      }

      // Post visualization - Edge Highlighting
      ImGui::Checkbox(vcString::Get("settingsVisEdge"), &pProgramState->settings.postVisualization.edgeOutlines.enable);
      if (pProgramState->settings.postVisualization.edgeOutlines.enable)
      {
        if (ImGui::SliderInt(vcString::Get("settingsVisEdgeWidth"), &pProgramState->settings.postVisualization.edgeOutlines.width, vcSL_EdgeHighlightMin, vcSL_EdgeHighlightMax))
          pProgramState->settings.postVisualization.edgeOutlines.width = udClamp(pProgramState->settings.postVisualization.edgeOutlines.width, vcSL_EdgeHighlightMin, vcSL_EdgeHighlightMax);

        // TODO: Make this less awful. 0-100 would make more sense than 0.0001 to 0.001.
        if (ImGui::SliderFloat(vcString::Get("settingsVisEdgeThreshold"), &pProgramState->settings.postVisualization.edgeOutlines.threshold, vcSL_EdgeHighlightThresholdMin, vcSL_EdgeHighlightThresholdMax, "%.3f", 2))
          pProgramState->settings.postVisualization.edgeOutlines.threshold = udClamp(pProgramState->settings.postVisualization.edgeOutlines.threshold, vcSL_EdgeHighlightThresholdMin, vcSL_EdgeHighlightThresholdMax);
        ImGui::ColorEdit4(vcString::Get("settingsVisEdgeColour"), &pProgramState->settings.postVisualization.edgeOutlines.colour.x);
      }

      // Post visualization - Colour by Height
      ImGui::Checkbox(vcString::Get("settingsVisHeight"), &pProgramState->settings.postVisualization.colourByHeight.enable);
      if (pProgramState->settings.postVisualization.colourByHeight.enable)
      {
        ImGui::ColorEdit4(vcString::Get("settingsVisHeightStartColour"), &pProgramState->settings.postVisualization.colourByHeight.minColour.x);
        ImGui::ColorEdit4(vcString::Get("settingsVisHeightEndColour"), &pProgramState->settings.postVisualization.colourByHeight.maxColour.x);

        // TODO: Set min/max to the bounds of the model? Currently set to 0m -> 1km with accuracy of 1mm
        if (ImGui::SliderFloat(vcString::Get("settingsVisHeightStart"), &pProgramState->settings.postVisualization.colourByHeight.startHeight, vcSL_ColourByHeightMin, vcSL_ColourByHeightMax, "%.3f"))
          pProgramState->settings.postVisualization.colourByHeight.startHeight = udClamp(pProgramState->settings.postVisualization.colourByHeight.startHeight, -vcSL_GlobalLimitf, vcSL_GlobalLimitf);
        if (ImGui::SliderFloat(vcString::Get("settingsVisHeightEnd"), &pProgramState->settings.postVisualization.colourByHeight.endHeight, vcSL_ColourByHeightMin, vcSL_ColourByHeightMax, "%.3f"))
          pProgramState->settings.postVisualization.colourByHeight.endHeight = udClamp(pProgramState->settings.postVisualization.colourByHeight.endHeight, -vcSL_GlobalLimitf, vcSL_GlobalLimitf);
      }

      // Post visualization - Colour by Depth
      ImGui::Checkbox(vcString::Get("settingsVisDepth"), &pProgramState->settings.postVisualization.colourByDepth.enable);
      if (pProgramState->settings.postVisualization.colourByDepth.enable)
      {
        ImGui::ColorEdit4(vcString::Get("settingsVisDepthColour"), &pProgramState->settings.postVisualization.colourByDepth.colour.x);

        // TODO: Find better min and max values? Currently set to 0m -> 1km with accuracy of 1mm
        if (ImGui::SliderFloat(vcString::Get("settingsVisDepthStart"), &pProgramState->settings.postVisualization.colourByDepth.startDepth, vcSL_ColourByDepthMin, vcSL_ColourByDepthMax, "%.3f"))
          pProgramState->settings.postVisualization.colourByDepth.startDepth = udClamp(pProgramState->settings.postVisualization.colourByDepth.startDepth, -vcSL_GlobalLimitf, vcSL_GlobalLimitf);
        if (ImGui::SliderFloat(vcString::Get("settingsVisDepthEnd"), &pProgramState->settings.postVisualization.colourByDepth.endDepth, vcSL_ColourByDepthMin, vcSL_ColourByDepthMax, "%.3f"))
          pProgramState->settings.postVisualization.colourByDepth.endDepth = udClamp(pProgramState->settings.postVisualization.colourByDepth.endDepth, -vcSL_GlobalLimitf, vcSL_GlobalLimitf);
      }

      // Post visualization - Contours
      ImGui::Checkbox(vcString::Get("settingsVisContours"), &pProgramState->settings.postVisualization.contours.enable);
      if (pProgramState->settings.postVisualization.contours.enable)
      {
        ImGui::ColorEdit4(vcString::Get("settingsVisContoursColour"), &pProgramState->settings.postVisualization.contours.colour.x);

        // TODO: Find better min and max values? Currently set to 0m -> 1km with accuracy of 1mm
        if (ImGui::SliderFloat(vcString::Get("settingsVisContoursDistances"), &pProgramState->settings.postVisualization.contours.distances, vcSL_ContourDistanceMin, vcSL_ContourDistanceMax, "%.3f", 2))
          pProgramState->settings.postVisualization.contours.distances = udClamp(pProgramState->settings.postVisualization.contours.distances, vcSL_ContourDistanceMin, vcSL_GlobalLimitSmallf);
        if (ImGui::SliderFloat(vcString::Get("settingsVisContoursBandHeight"), &pProgramState->settings.postVisualization.contours.bandHeight, vcSL_ContourBandHeightMin, vcSL_ContourBandHeightMax, "%.3f", 2))
          pProgramState->settings.postVisualization.contours.bandHeight = udClamp(pProgramState->settings.postVisualization.contours.bandHeight, vcSL_ContourBandHeightMin, vcSL_GlobalLimitSmallf);
      }
    }
  }
  ImGui::End();
}
