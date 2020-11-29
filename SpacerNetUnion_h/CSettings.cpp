// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	CSetting::CSetting(CSettingType type, CString section, CString name, CString default_value)
	{
		this->type = type;
		this->section = section;
		this->name = name;
		this->default_value = default_value;
	}



	float CSettings::GetFloatVal(CString key)
	{
		auto& pair = list[key];
		float result = 0.0f;
		if (!pair.IsNull())
		{
			result = pair.GetValue()->value_float;
		}

		return result;
	}

	int CSettings::GetIntVal(CString key)
	{
		auto& pair = list[key];
		int result = 0;
		if (!pair.IsNull())
		{
			result = pair.GetValue()->value_int;
		}

		return result;
	}

	CString CSettings::GetVal(CString key)
	{
		auto& pair = list[key];
		CString result = "";

		if (!pair.IsNull())
		{
			result = pair.GetValue()->value_string;
		}

		return result;
	}

	void CSettings::SetFloatVal(CString key, float val)
	{
		auto& pair = list[key];

		if (!pair.IsNull())
		{
			pair.GetValue()->value_float = val;
			pair.GetValue()->value_string = CString(val);
		}
	}

	void CSettings::SetIntVal(CString key, int val)
	{
		auto& pair = list[key];

		if (!pair.IsNull())
		{
			pair.GetValue()->value_int = val;
			pair.GetValue()->value_string = CString(val);
		}
	}

	void CSettings::SetStringVal(CString key, CString val)
	{
		auto& pair = list[key];

		if (!pair.IsNull())
		{
			pair.GetValue()->value_string = val;
		}
	}



	void CSettings::Load()
	{
		CString value;

		WriteLine("CSettings: reading settings...");

		auto arr = list.GetArray();

		for (uint i = 0; i < arr.GetNum(); i++)
		{
			CSetting* set = arr.GetSafe(i)->GetValue();

			if (set)
			{
				theApp.spcOpt.Read(value, set->section, set->name, set->default_value);

				set->value_string = value.ToChar();

				if (set->type == TYPE_INT)
				{
					set->value_int = value.ToInt32();
				}
				else if (set->type == TYPE_FLOAT)
				{
					set->value_float = value.ToReal32();
				}
			}
		}
	}

	void CSettings::Save()
	{
		CString value;

		auto arr = list.GetArray();

		for (uint i = 0; i < arr.GetNum(); i++)
		{
			CSetting* set = arr.GetSafe(i)->GetValue();

			if (set)
			{
				theApp.spcOpt.Write(set->value_string, set->section, set->name);
			}
		}


		auto getVersion = (callVoidFunc)GetProcAddress(theApp.module, "GetSpacerVersion");
		getVersion();

		CString version = Stack_PeekString();

		theApp.spcOpt.Write(version, "INFO", "Version");

		theApp.spcOpt.SaveAll();
	}

	void CSettings::Apply()
	{

		if (ogame->GetWorld())
		{
			zCVob::s_renderVobs = GetIntVal("showVobs");

			ogame->GetWorld()->showWaynet = GetIntVal("showWaynet");

			zCVobLight::renderLightVisuals = GetIntVal("showHelpVobs");		// �������� ������� ��� ���������� �����
			zCVob::SetShowHelperVisuals(zCVobLight::renderLightVisuals);          // �������� ������� ��� ���������, ���������� ����� � �������

			ogame->GetWorld()->bspTree.drawVobBBox3D = GetIntVal("drawBBoxGlobal");	

			int maxFps = GetIntVal("maxFPS");

			if (maxFps > 0)
			{
				ztimer->LimitFPS(maxFps);
			}

			if (zmusic) zmusic->SetVolume((float)GetIntVal("musicVolume") / 100.0f);


			theApp.turnWpMode = (TurnWpMode)GetIntVal("wpTurnOn");
			SetRangeVobs();
			SetRangeWorld();

			int pickMode = theApp.options.GetIntVal("bTogglePickMaterial") ? 1 : 0;

			if (!pickMode)
			{
				mm.CleanSelectMaterial();
			}
		}

		this->Save();

	}



	
	void CSettings::Init()
	{

		CSetting* set = NULL;

		set = new CSetting(TYPE_INT, "CAMERA", "camTransSpeed", "16");
		list.Insert("camTransSpeed", set);

		set = new CSetting(TYPE_INT, "CAMERA", "camRotSpeed", "16");
		list.Insert("camRotSpeed", set);

		set = new CSetting(TYPE_INT, "CAMERA", "rangeWorld", "2000");
		list.Insert("rangeWorld", set);

		set = new CSetting(TYPE_INT, "CAMERA", "rangeVobs", "2000");
		list.Insert("rangeVobs", set);

		set = new CSetting(TYPE_INT, "CAMERA", "showFps", "1");
		list.Insert("showFps", set);

		set = new CSetting(TYPE_INT, "CAMERA", "showTris", "1");
		list.Insert("showTris", set);

		set = new CSetting(TYPE_INT, "CAMERA", "slerpRot", "0");
		list.Insert("slerpRot", set);


		set = new CSetting(TYPE_INT, "CAMERA", "showVobsCount", "1");
		list.Insert("showVobsCount", set);


		set = new CSetting(TYPE_INT, "CAMERA", "showWaypointsCount", "1");
		list.Insert("showWaypointsCount", set);

		set = new CSetting(TYPE_INT, "CAMERA", "showCamCoords", "1");
		list.Insert("showCamCoords", set);

		set = new CSetting(TYPE_INT, "CAMERA", "showVobDist", "1");
		list.Insert("showVobDist", set);

		set = new CSetting(TYPE_INT, "CAMERA", "showInvisibleVobs", "0");
		list.Insert("showInvisibleVobs", set);

		set = new CSetting(TYPE_INT, "CAMERA", "bTogglePickMaterial", "0");
		list.Insert("bTogglePickMaterial", set);


		set = new CSetting(TYPE_INT, "CAMERA", "hideCamWindows", "1");
		list.Insert("hideCamWindows", set);

		set = new CSetting(TYPE_INT, "CAMERA", "showWPNames", "1");
		list.Insert("showWPNames", set);

		set = new CSetting(TYPE_INT, "CAMERA", "previewSpeed", "20");
		list.Insert("previewSpeed", set);

		



		set = new CSetting(TYPE_INT, "SPACER", "showVobs", "1");
		list.Insert("showVobs", set);

		set = new CSetting(TYPE_INT, "SPACER", "autoCompileWorldAfterLoad", "0");
		list.Insert("autoCompileWorldAfterLoad", set);

		set = new CSetting(TYPE_INT, "SPACER", "showWaynet", "1");
		list.Insert("showWaynet", set);

		set = new CSetting(TYPE_INT, "SPACER", "showHelpVobs", "1");
		list.Insert("showHelpVobs", set);

		set = new CSetting(TYPE_INT, "SPACER", "drawBBoxGlobal", "0");
		list.Insert("drawBBoxGlobal", set);

		set = new CSetting(TYPE_INT, "SPACER", "addDatePrefix", "1");
		list.Insert("addDatePrefix", set);

		set = new CSetting(TYPE_INT, "SPACER", "maxFPS", "0");
		list.Insert("maxFPS", set);

		set = new CSetting(TYPE_INT, "SPACER", "vobListRadius", "150");
		list.Insert("vobListRadius", set);

		set = new CSetting(TYPE_INT, "SPACER", "showModelPreview", "1");
		list.Insert("showModelPreview", set);

		set = new CSetting(TYPE_INT, "SPACER", "searchOnly3DS", "0");
		list.Insert("searchOnly3DS", set);

		set = new CSetting(TYPE_INT, "SPACER", "musicVolume", "100");
		list.Insert("musicVolume", set);

		set = new CSetting(TYPE_INT, "SPACER", "musicZenOff", "0");
		list.Insert("musicZenOff", set);

		set = new CSetting(TYPE_INT, "SPACER", "askExitZen", "1");
		list.Insert("askExitZen", set);

		set = new CSetting(TYPE_INT, "SPACER", "lightCompileType", "0");
		list.Insert("lightCompileType", set);


		set = new CSetting(TYPE_INT, "SPACER", "lightCompileRegionOn", "0");
		list.Insert("lightCompileRegionOn", set);

		set = new CSetting(TYPE_INT, "SPACER", "lightCompileRadius", "2000");
		list.Insert("lightCompileRadius", set);


		set = new CSetting(TYPE_INT, "SPACER", "worldCompileType", "0");
		list.Insert("worldCompileType", set);


		set = new CSetting(TYPE_INT, "SPACER", "addWPToNet", "1");
		list.Insert("addWPToNet", set);

		set = new CSetting(TYPE_INT, "SPACER", "addWPAutoName", "1");
		list.Insert("addWPAutoName", set);

		set = new CSetting(TYPE_INT, "SPACER", "downFPToGround", "1");
		list.Insert("downFPToGround", set);

		set = new CSetting(TYPE_INT, "SPACER", "addFPAutoName", "1");
		list.Insert("addFPAutoName", set);

		set = new CSetting(TYPE_INT, "SPACER", "fullPathTitle", "0");
		list.Insert("fullPathTitle", set);


		set = new CSetting(TYPE_STRING, "PATH", "treeVobPath", "");
		list.Insert("treeVobPath", set);
		set = new CSetting(TYPE_STRING, "PATH", "meshPath", "");
		list.Insert("meshPath", set);
		set = new CSetting(TYPE_STRING, "PATH", "zenzPath", "");
		list.Insert("zenzPath", set);
		set = new CSetting(TYPE_STRING, "PATH", "vobsPath", "");
		list.Insert("vobsPath", set);
		set = new CSetting(TYPE_STRING, "PATH", "vobResPath", "");
		list.Insert("vobResPath", set);

		set = new CSetting(TYPE_INT, "PATH", "openLastZen", "0");
		list.Insert("openLastZen", set);

		set = new CSetting(TYPE_STRING, "PATH", "openLastZenPath", "");
		list.Insert("openLastZenPath", set);



		set = new CSetting(TYPE_INT, "CONTROLS", "vobTransSpeed", "60");
		list.Insert("vobTransSpeed", set);

		set = new CSetting(TYPE_INT, "CONTROLS", "vobRotSpeed", "32");
		list.Insert("vobRotSpeed", set);


		set = new CSetting(TYPE_INT, "CONTROLS", "vobInsertItemLevel", "1");
		list.Insert("vobInsertItemLevel", set);

		set = new CSetting(TYPE_INT, "CONTROLS", "vobInsertVobRotRand", "0");
		list.Insert("vobInsertVobRotRand", set);

		set = new CSetting(TYPE_INT, "CONTROLS", "vobInsertHierarchy", "1");
		list.Insert("vobInsertHierarchy", set);


		set = new CSetting(TYPE_INT, "CONTROLS", "wpTurnOn", "0");
		list.Insert("wpTurnOn", set);


		set = new CSetting(TYPE_INT, "CONTROLS", "selectMoveWhenVobInsert", "0");
		list.Insert("selectMoveWhenVobInsert", set);
	}

	CSettings::~CSettings()
	{
		list.Clear();
	}

}


