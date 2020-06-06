// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	//RX_FIX
	int& playerLightInt = *(int*)0x0089EBB4;


	double GetAngleBetweenVobs(zCVob* vob, zCVob* target, zVEC3 realUnit)
	{
		zVEC3 targetRel = (target->GetPositionWorld() - vob->GetPositionWorld()).Normalize();

		double angle = acos((realUnit.n[0] * targetRel.n[0] + realUnit.n[2] * targetRel.n[2])
			/ (targetRel.Length() * realUnit.Length())) * 180 / PI;


		return angle;
	}


	void __cdecl PlaySoundGame(class zSTRING &)
	{
		XCALL(0x006CBFD0);
	}



	int GetFPS(void)
	{
		XCALL(0x004FDCD0);
	}


	void __cdecl sysEvent()
	{
		XCALL(0x005053E0)
	}

	// ������������� ����� �����
	void SetTime(int hour, int min)
	{
		int day, hour_, min_;

		ogame->GetTime(day, hour_, min_);
		ogame->SetTime(day, hour, min);
	}

	int printDebugCount = 0;

	void PrintDebug(zSTRING str) {

		screen->Print(F(1), F(1) + (F(3) * printDebugCount++), str);
	}

	void ResetPrintDebug()
	{
		printDebugCount = 0;
	}



	void OutFile(zSTRING str, bool close)
	{
		static std::ofstream fout;
		static bool enable = false;

		if (enable)
		{
			if (!fout.is_open())
			{
				fout.open("./system/_SpacerNetErrors.txt", std::ios_base::app);
			}

			fout << "Union: " << str.ToChar() << std::endl;

			if (close)
			{
				fout.close();
			}
		}

		std::cout << "Union: " << str.ToChar() << std::endl;

	}

	void ClearLMB()
	{
		*(int*)0x8D1668 = 0;
	}


	int childrenCountVob = 0;

	void GetChildrenCount(zCTree<zCVob>* node)
	{
		zCVob* vob = node->GetData();

		childrenCountVob++;
		node = node->GetFirstChild();
		while (node != NULL)
		{
			GetChildrenCount(node);
			node = node->GetNextChild();

		}
	}


	zSTRING GetVobNameSafe(zCVob* vob)
	{
		zSTRING result = "";

		if (vob)
		{
			if (!vob->GetVobName().IsEmpty())
			{
				result = vob->GetVobName();
			}
			else if (vob->GetVisual())
			{
				result = vob->GetVisual()->GetVisualName();
			}
			else
			{
				result = "vob1";
			}

			result = result + " (" + vob->_GetClassDef()->GetClassName_() + ")";
		}

		return result;
	}
	zSTRING GetVobName(zCVob* vob)
	{
		zSTRING str = "";

		if (vob)
		{
			if (dynamic_cast<zCVobLevelCompo*>(vob))
			{
				if (vob->GetVisual())
				{

					str = vob->GetVisual()->GetVisualName().ToChar();
				}
				else
				{
					str = vob->GetVobName().ToChar();
				}

				str = str + " [Children Count: ";

				childrenCountVob = 0;
				GetChildrenCount(vob->globalVobTreeNode);

				str = str + ToStr(childrenCountVob - 1);
				str = str + "]";
			}
			else
			{
				// generate the name of the item	
				if (!vob->GetVobName().IsEmpty())
				{
					str = vob->GetVobName();
				}
				else if (vob->GetVisual())
				{
					str = vob->GetVisual()->GetVisualName();
				}
				else
				{
					str = "vob1";
				}

				str = str + " (";
				str = str + vob->_GetClassDef()->GetClassName_();
				str = str + ")";
			}




		}

		return str;
	}

	// ���������� ����� ������������ ������
	void DrawLine(zCCamera *cam, zCVob* v1, zCVob* v2, zCOLOR col)
	{
		if (!cam || !v1 || !v2) return;

		zVEC3 wsPoint1, wsPoint2, csPoint1, csPoint2;	// world/camera-space
		zVEC3 ssPoint1, ssPoint2;						// screen-space X,Y

		wsPoint1 = v1->GetPositionWorld();
		wsPoint2 = v2->GetPositionWorld();
		//RX_FIX
		//csPoint1 = cam->Transform(wsPoint1);
		//csPoint2 = cam->Transform(wsPoint2);
		Alg_ClipAtZ0(csPoint1, csPoint2);

		if (csPoint1[VZ] >= 0)
		{
			cam->Project(&csPoint1, ssPoint1[VX], ssPoint1[VY]);
		}
		else return;

		if (csPoint2[VZ] >= 0)
		{
			cam->Project(&csPoint2, ssPoint2[VX], ssPoint2[VY]);
		}
		else return;

		float camSpaceInf1 = (csPoint1[VZ] != 0 ? 1 / csPoint1[VZ] : 0);
		float camSpaceInf2 = (csPoint1[VZ] != 0 ? 1 / csPoint2[VZ] : 0);

		int x1 = (int)ssPoint1[VX];
		int y1 = (int)ssPoint1[VY];
		int x2 = (int)ssPoint2[VX];
		int y2 = (int)ssPoint2[VY];
		if (!screen->ClipLine(x1, y1, x2, y2)) return;

		if (zrenderer) zrenderer->DrawLineZ((float)x1, (float)y1, camSpaceInf1, (float)x2, (float)y2, camSpaceInf2, col);

		if (csPoint1[VZ]<1000)
		{
			screen->Print(screen->anx(x1), screen->any(y1), v1->GetVobName());
		}
		if (csPoint2[VZ]<1000)
		{
			screen->Print(screen->anx(x2), screen->any(x2), v2->GetVobName());
		}
	}

	bool replace(std::string& str, const std::string& from, const std::string& to) {
		size_t start_pos = str.find(from);
		if (start_pos == std::string::npos)
			return false;
		str.replace(start_pos, from.length(), to);
		return true;
	}

	void CreateFileList()
	{
		char** fileList = Null;

		uint numInList = Vdfs32::vdf_filelist_virtual(fileList);


		for (size_t i = 0; i < numInList; i++)
		{
			std::string str(fileList[i]);
			theApp.modelsStrVDF.InsertEnd(&zSTRING(str.c_str()));
		}



		for (size_t i = 0; i < numInList; i++)
			delete fileList[i];

		delete fileList;




		char** fileListWork = Null;

		numInList = Vdfs32::vdf_filelist_physical(fileListWork);


		for (size_t i = 0; i < numInList; i++)
		{
			std::string str(fileListWork[i]);
			theApp.modelsStrWORK.InsertEnd(&zSTRING(str.c_str()));

		}

		for (size_t i = 0; i < numInList; i++)
			delete fileListWork[i];

		delete fileListWork;

		//MessageBox(0, zSTRING("������ ������!").ToChar(), 0, 0);
	}


	void SetBBoxValue(int x, int y, int z)
	{
		zCVob* vob = theApp.GetSelectedVob();

		if (vob)
		{
			vob->AddRef();

			zTBBox3D box;
			box = vob->GetBBox3DWorld();

			zVEC3 centerPt = vob->GetPositionWorld();

			box.maxs.n[VX] = centerPt.n[VX] + int(x / 2);
			box.mins.n[VX] = centerPt.n[VX] - int(x / 2);

			box.maxs.n[VY] = centerPt.n[VY] + int(y / 2);
			box.mins.n[VY] = centerPt.n[VY] - int(y / 2);

			box.maxs.n[VZ] = centerPt.n[VZ] + int(z / 2);
			box.mins.n[VZ] = centerPt.n[VZ] - int(z / 2);

			vob->SetBBox3DWorld(box);


			zRELEASE(vob);
		}
		theApp.SelectObject(theApp.pickedVob);
		//theApp.SetSelectedVob(NULL, "SetBBoxValue");
	}

	float GetBboxValue(int coord)
	{
		zCVob* vob = theApp.GetSelectedVob();

		float result = 0;

		if (vob)
		{
			vob->AddRef();

			zVEC3 centerPt = vob->GetPositionWorld();
			zTBBox3D box;
			box = vob->GetBBox3DWorld();

			switch (coord)
			{
			case 0: result = (float)((int)(box.maxs.n[VX] - box.mins.n[VX])); break;
			case 1: result = (float)((int)(box.maxs.n[VY] - box.mins.n[VY])); break;
			case 2: result = (float)((int)(box.maxs.n[VZ] - box.mins.n[VZ])); break;
			}

			zRELEASE(vob);
		}

		return result;
	}

	bool CheckUniqNameExist(CString name)
	{
		bool result = false;

		zSTRING vobName = zSTRING(name).Upper();

		if (theApp.pickedVob)
		{
			if (dynamic_cast<zCMover*>(theApp.pickedVob)
				|| dynamic_cast<zCVobSpot*>(theApp.pickedVob)
				|| dynamic_cast<zCVobWaypoint*>(theApp.pickedVob))
			{


				if (vobName.Length() == 0)
				{
					result = true;
				}



				zCWayNet* waynet = ogame->GetWorld()->GetWayNet();

				if (waynet)
				{
					zCWaypoint* found = waynet->GetWaypoint(vobName);

					if (found)
					{
						result = true;
					}
				}


				zCVob* vob = ogame->GetWorld()->SearchVobByName(vobName);

				if (vob)
				{
					result = true;
				}
			}
		}


		return result;
	}

	bool CheckVobNameExist(CString name, bool isWaypoint)
	{
		bool result = false;


		//std::cout << "CheckVobNameExist: looking for " << name << std::endl;


		zCWayNet* waynet = ogame->GetWorld()->GetWayNet();

		if (waynet)
		{
			zCWaypoint* found = waynet->GetWaypoint(zSTRING(name).Upper());

			if (found)
			{
				//std::cout << "CheckVobNameExist: found waypoint " << name << std::endl;
				result = true;
			}
		}


		zCVob* vob = ogame->GetWorld()->SearchVobByName(zSTRING(name).Upper());

		if (vob)
		{
			//std::cout << "CheckVobNameExist: found vob " << name << std::endl;
			result = true;
		}

		return result;
	}


	void SetRangeVobs()
	{
		float drawDist = ((float)theApp.options.GetIntVal("rangeVobs") / 1000);

		ogame->GetWorld()->SetVobFarClipZScalability(drawDist);
	}

	void SetRangeWorld()
	{
		float drawDist = ((float)theApp.options.GetIntVal("rangeWorld") / 1000);

		if (zCSkyControler::s_activeSkyControler)
		{
			zCSkyControler::s_activeSkyControler->SetFarZ(drawDist * 10000);
			zCSkyControler::s_activeSkyControler->SetFarZScalability(drawDist);
		}

	}


	void FindNearestVobs()
	{
		zCWorld* world = ogame->GetWorld();
		zCVob* pickedVob = theApp.GetSelectedVob();
		zCClassDef* lightClassdef = zCVob::classDef;



		zTBBox3D box;

		zCVob* camVob = ogame->GetCamera()->connectedVob;

		zVEC3 normCamera = camVob->GetAtVectorWorld().Normalize();








		(callVoidFunc)GetProcAddress(theApp.module, "ClearVobList")();

		auto GetSearchRadius = (voidFuncPointer)GetProcAddress(theApp.module, "GetSearchRadius");

		int radius = GetSearchRadius();

		box.maxs = camVob->GetPositionWorld() + zVEC3(radius, radius, radius);
		box.mins = camVob->GetPositionWorld() - zVEC3(radius, radius, radius);

		zCArray<zCVob*>resVobList;

		ogame->GetWorld()->CollectVobsInBBox3D(resVobList, box);

		//ogame->GetWorld()->SearchVobListByBaseClass(lightClassdef, resVobList, 0);

		for (int i = 0; i < resVobList.GetNumInList(); i++)
		{
			zCVob* nextVob = resVobList.GetSafe(i);


			if (nextVob && (nextVob->GetDistanceToVob(*ogame->GetCamera()->connectedVob) <= radius * 2) && nextVob != ogame->GetCamera()->connectedVob)
			{
				addToVobList addEntry = (addToVobList)GetProcAddress(theApp.module, "AddToVobList");
				Stack_PushString(GetVobName(nextVob));
				addEntry((uint)nextVob);
				//vc->AddVob(nextVob);
			}
		}
	}

	void WriteLine(char* message)
	{
		std::cout << (std::string("Union: ").append(message)).c_str() << std::endl;
	}


	int GetRandVal(int min, int max)
	{
		return (rand() % (max - min + 1) + min);
	}


	void GetDesktopResolution(int& horizontal, int& vertical)
	{
		RECT desktop;
		// Get a handle to the desktop window
		const HWND hDesktop = GetDesktopWindow();
		// Get the size of screen to the variable desktop
		GetWindowRect(hDesktop, &desktop);
		// The top left corner will have coordinates (0,0)
		// and the bottom right corner will have coordinates
		// (horizontal, vertical)
		horizontal = desktop.right;
		vertical = desktop.bottom;
	}


	void ToggleMusic(bool mode)
	{

		if (mode)
		{
			zoptions->WriteInt("SOUND", "musicEnabled", 1, 0);
			zCMusicSystem::DisableMusicSystem(FALSE);
			zCZoneMusic::SetAutochange(TRUE);
		}
		else
		{
			zoptions->WriteInt("SOUND", "musicEnabled", 0, 0);
			zCZoneMusic::SetAutochange(FALSE);
			zCMusicSystem::DisableMusicSystem(TRUE);
		}
	}

	zSTRING GetFileExtension(zSTRING name)
	{
		std::string filename(name);
		std::string::size_type idx;

		idx = filename.rfind('.');

		if (idx != std::string::npos)
		{
			std::string extension = filename.substr(idx + 1);

			return zSTRING(extension.c_str()).Upper();
		}
		else
		{
			return "";
		}
	}

	uint vTableArr[] = { 0x0082E6F0, 0x0082E6FC, 0x0082E75C, 0x0082E7BC, 0x0082E81C, 0x0082E89C, 0x0082E8DC, 0x0082E904, 0x0082E970, 0x0082E9B4, 0x0082EA4C, 0x0082EB5C, 0x0082EBC4, 0x0082EC2C, 0x0082EC94, 0x0082ECF8, 0x0082ED00, 0x0082ED14, 0x0082ED78, 0x0082ED8C, 0x0082EE60, 0x0082EE8C, 0x0082EF20, 0x0082EF44, 0x0082EF9C, 0x0082EFD8, 0x0082EFEC, 0x0082F00C, 0x0082F044, 0x0082F09C, 0x0082F0EC, 0x0082F0F8, 0x0082F104, 0x0082F148, 0x0082F1EC, 0x0082F278, 0x0082F2D8, 0x0082F2EC, 0x0082F300, 0x0082F384, 0x0082F404, 0x0082F484, 0x0082F520, 0x0082F52C, 0x0082F594, 0x0082F5F0, 0x0082F61C, 0x0082F70C, 0x0082F80C, 0x0082F814, 0x0082F924, 0x0082F92C, 0x0082FA38, 0x0082FA80, 0x0082FA88, 0x0082FACC, 0x0082FB54, 0x0082FB70, 0x0082FBA8, 0x0082FBB0, 0x0082FBB8, 0x0082FBEC, 0x0082FC30, 0x0082FC50, 0x0082FC6C, 0x0082FCA0, 0x0082FCAC, 0x0082FED0, 0x0082FEDC, 0x0082FF24, 0x0082FF2C, 0x0082FF78, 0x0082FF84, 0x0082FFDC, 0x008300B4, 0x00830104, 0x0083016C, 0x008301BC, 0x00830208, 0x00830210, 0x00830218, 0x00830260, 0x0083029C, 0x008303DC, 0x00830450, 0x00830484, 0x00830534, 0x00830690, 0x00830874, 0x008308EC, 0x0083096C, 0x00830A04, 0x00830A64, 0x00830AD4, 0x00830B64, 0x00830C3C, 0x00830CA4, 0x00830CAC, 0x00830D34, 0x00830D3C, 0x00830DC4, 0x00830DCC, 0x00830E54, 0x00830E5C, 0x00830EE8, 0x00830EF4, 0x00830F7C, 0x00830F84, 0x00831010, 0x0083101C, 0x008310C4, 0x008310D0, 0x008310E4, 0x0083113C, 0x0083115C, 0x00831200, 0x0083120C, 0x00831294, 0x0083131C, 0x0083137C, 0x00831404, 0x0083140C, 0x0083151C, 0x008316A8, 0x008316D4, 0x0083184C, 0x008319DC, 0x00831BA4, 0x00831BB4, 0x00831D44, 0x00831D4C, 0x00831D78, 0x00831DAC, 0x00831DCC, 0x00831DEC, 0x00831E0C, 0x00831E2C, 0x00831E4C, 0x00831E6C, 0x00831EA0, 0x00831ED4, 0x00831F54, 0x00831F78, 0x00831FA4, 0x00831FC8, 0x00832084, 0x00832100, 0x00832128, 0x008321E4, 0x00832214, 0x00832228, 0x0083225C, 0x00832290, 0x008322BC, 0x0083234C, 0x008323BC, 0x00832438, 0x00832498, 0x008324B0, 0x008324D4, 0x00832544, 0x008325C4, 0x00832620, 0x008326B4, 0x0083271C, 0x00832964, 0x008329CC, 0x00832A3C, 0x00832A64, 0x00832BC8, 0x00832BE8, 0x00832C0C, 0x00832C4C, 0x00832CBC, 0x00832D54, 0x00832DEC, 0x00832E84, 0x00832F54, 0x00833024, 0x0083305C, 0x00833078, 0x008330A8, 0x008330D8, 0x0083311C, 0x0083314C, 0x0083317C, 0x008331B4, 0x008331C8, 0x008331F0, 0x00833200, 0x00833210, 0x0083988C, 0x008398EC, 0x00839904, 0x0083997C, 0x00839A04, 0x00839A74, 0x00839B4C, 0x00839BC4, 0x00839C3C, 0x00839CB4, 0x00839D2C, 0x00839DA4, 0x00839E1C, 0x00839E94, 0x00839F14, 0x00839F94, 0x0083A00C, 0x0083A084, 0x0083A0FC, 0x0083A174, 0x0083A1EC, 0x0083A264, 0x0083A2DC, 0x0083A33C, 0x0083A39C, 0x0083A3FC, 0x0083A47C, 0x0083A4FC, 0x0083A5A4, 0x0083A630, 0x0083A6A4, 0x0083A72C, 0x0083A784, 0x0083A7E4, 0x0083A7FC, 0x0083A880, 0x0083A89C, 0x0083A924, 0x0083A9AC, 0x0083AA34, 0x0083AABC, 0x0083AB44, 0x0083ABCC, 0x0083AC54, 0x0083ACDC, 0x0083ACEC, 0x0083AD8C, 0x0083AE1C, 0x0083AEBC, 0x0083AF34, 0x0083B0C4, 0x0083B264, 0x0083B404, 0x0083B454, 0x0083B484, 0x0083B4CC, 0x0083B544, 0x0083B5A8, 0x0083B69C, 0x0083B74C, 0x0083B754, 0x0083B77C, 0x0083B7D0, 0x0083B7D8, 0x0083B804, 0x0083B858, 0x0083B860, 0x0083B88C, 0x0083B8E4, 0x0083B8EC, 0x0083B914, 0x0083B970, 0x0083B978, 0x0083B9A4, 0x0083BA08, 0x0083BA10, 0x0083BA3C, 0x0083BA9C, 0x0083BAA4, 0x0083BACC, 0x0083BB48, 0x0083BB84, 0x0083BBAC, 0x0083BBEC, 0x0083BC08, 0x0083BC10, 0x0083BC3C, 0x0083BC90, 0x0083BCB8, 0x0083BCEC, 0x0083BD14, 0x0083BD48, 0x0083BD70, 0x0083BD98, 0x0083BDC0, 0x0083BE2C, 0x0083BE7C, 0x0083BEB0, 0x0083BF00, 0x0083BF38, 0x0083BF6C, 0x0083BFA4, 0x0083BFDC, 0x0083C010, 0x0083C064, 0x0083C10C, 0x0083C1C8, 0x0083C218, 0x0083C220, 0x0083C28C, 0x0083C364, 0x0083C36C, 0x0083C43C, 0x0083C44C, 0x0083C4AC, 0x0083C574, 0x0083C644, 0x0083C714, 0x0083C804, 0x0083C8E0, 0x0083C8EC, 0x0083C914, 0x0083CA3C, 0x0083CB64, 0x0083CCBC, 0x0083CE04, 0x0083CF2C, 0x0083D054, 0x0083D19C, 0x0083D2CC, 0x0083D3FC, 0x0083D474, 0x0083D4D4, 0x0083D58C, 0x0083D6D0, 0x0083D6DC, 0x0083D710, 0x0083D724, 0x0083DA8C, 0x0083DAFC, 0x0083DB6C, 0x0083DBDC, 0x0083DC4C, 0x0083DCBC, 0x0083DD2C, 0x0083DD9C, 0x0083DE0C, 0x0083DE7C, 0x0083DF08, 0x0083DF80, 0x0083DFE4, 0x0083E08C, 0x0083E120, 0x0083E138, 0x0083E174, 0x0083E220, 0x0083E23C, 0x0083E2FC, 0x0083E304, 0x0083E30C, 0x0083E344, 0x0083E364, 0x0083E3D8, 0x0083E3EC, 0x0083E410, 0x0083E428, 0x0083E434, 0x0083E4AC, 0x00842368, 0x008423FC, 0x00842584, 0x00842598, 0x00842AF8, 0x008437D8 };

	bool IsValidZObject(void* ptr)
	{
		if (shi_msize(ptr) != -1)
		{
			uint vt_address = *(uint*)ptr;

			int count = (sizeof(vTableArr) / sizeof(uint));

			for (int i = 0; i < count; i++)
			{
				if (vTableArr[i] == vt_address)
				{
					return true;
				}
			}

		}

		return false;
	}




	CString GetLang(CString key)
	{
		Stack_PushString(key);

		int codePage = 1251;
		static auto caller = (voidFuncPointer)GetProcAddress(theApp.module, "GetLangString");
		codePage = caller();
		return Stack_PeekStringW().WToA(codePage);
	}
}