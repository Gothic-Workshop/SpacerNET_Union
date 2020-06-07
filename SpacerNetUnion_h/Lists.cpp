// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	// ������ ������ � ����
	void SpacerApp::CreateItemsList()
	{
		auto addItem = (callVoidFunc)GetProcAddress(theApp.module, "AddItemToList");

		zSTRING scriptClassName = "C_ITEM";

		int pos, typ, anz, c, classindex;

		classindex = parser->GetIndex(scriptClassName);
		if (classindex != -1)
		{
			parser->GetSymbolInfo(classindex, typ, anz);
			pos = parser->GetInstance(classindex, 0);


			while (pos >= 0)
			{
				zSTRING s = parser->GetSymbolInfo(pos, typ, c);
				// instance_list.Insert(s);
				Stack_PushString(s.Upper());
				addItem();
				pos = parser->GetInstance(classindex, pos + 1);
			}
		}

		auto sort = (voidFuncPointer)GetProcAddress(theApp.module, "SortItems");
		sort();
	}


	void SpacerApp::CreateMobNameList()
	{
		zCArray<zSTRING> mobNames;


		cmd << "Looking for mobnames..." << endl;


		//MOBNAME_SIGH
		for (int i = 0; i < parser->symtab.table.GetNumInList(); i++)
		{
			if (parser->symtab.table.GetSafe(i)->name.Search("MOBNAME_", 1) != -1)
			{
				mobNames.Insert(parser->symtab.table.GetSafe(i)->name);
			}
		}

		for (int i = 0; i < mobNames.GetNumInList(); i++)
		{
			//cmd << CString(mobNames.GetSafe(i)) << endl;
		}

	}
	// ������ ���� �������� �� �������� ������� ����
	void SpacerApp::CreateVisualsList()
	{


		CreateFileList();

		auto addVisuals = (callVoidFunc)GetProcAddress(theApp.module, "AddVisualToListVDF");

		for (int i = 0; i < modelsStrVDF.GetNum(); i++)
		{
			Stack_PushString(modelsStrVDF.GetSafe(i).Upper());
			addVisuals();
		}


		addVisuals = (callVoidFunc)GetProcAddress(theApp.module, "AddVisualToListWORK");

		for (int i = 0; i < modelsStrWORK.GetNum(); i++)
		{
			Stack_PushString(modelsStrWORK.GetSafe(i));
			addVisuals();
		}



		auto sort = (callVoidFunc)GetProcAddress(theApp.module, "SortVisuals");
		sort();

		modelsStrWORK.DeleteList();
		modelsStrVDF.DeleteList();
	}

	// ������ PFX � ���� CreateVisualsList
	void SpacerApp::CreatePFXList()
	{
		auto addPFX = (callVoidFunc)GetProcAddress(theApp.module, "AddPacticleToList");

		for (int i = 0; i<zCParticleFX::s_emitterPresetList.GetNumInList(); i++)
		{
			Stack_PushString(zCParticleFX::s_emitterPresetList.GetSafe(i)->particleFXName.Upper());
			addPFX();
		}
		auto sort = (callVoidFunc)GetProcAddress(theApp.module, "SortPFX");
		sort();
	}

	// ������ Sounds � ����
	void SpacerApp::CreateSoundList()
	{
		auto addSound = (callVoidFunc)GetProcAddress(theApp.module, "AddSoundToList");


		zCParser* soundParser = zsound->GetSFXParser();;
		soundParser->SetStopOnError(FALSE);
		zoptions->ChangeDir(DIR_SCRIPTS);

		zSTRING classname = "C_SFX";

		int pos, typ, anz, c, classindex;
		zSTRING s;

		classindex = soundParser->GetIndex(classname);
		if (classindex != -1)
		{
			// Get instances
			soundParser->GetSymbolInfo(classindex, typ, anz);
			pos = soundParser->GetInstance(classindex, 0);
			while (pos >= 0)
			{
				s = soundParser->GetSymbolInfo(pos, typ, c);
				Stack_PushString(s);
				addSound();
				pos = soundParser->GetInstance(classindex, pos + 1);
			}

		}



		(voidFuncPointer)GetProcAddress(theApp.module, "SortSounds")();
	}

	void SpacerApp::BuildTree()
	{
		zCWorld* world = ogame->GetWorld();

		(callVoidFunc)GetProcAddress(theApp.module, "ClearAllEntries")();


		zCTree<zCVob>* tree = world->globalVobTree.GetFirstChild();
		while (tree)
		{
			CreateBaseVobTree(tree);
			tree = tree->GetNextChild();
		}
		treeIsReady = true;
		std::cout << "Union: Creating TreeView in C#..." << std::endl;
		CreateTree();





	}


	void CreateBaseVobTreeNew(zCTree<zCVob>* node)
	{
		zCVob* vob = node->GetData();


		if (vob)
		{

			if (vob->GetVisual())
			{
				vob->GetVisual()->Release();
			}
			vob->SetShowVisual(FALSE);

			vob->RemoveVobFromBspTree();
			//if ((vob->GetVisual()) && (vob->GetHomeWorld()))	vob->GetVisual()->HostVobRemovedFromWorld(vob, vob->GetHomeWorld());
			//vob->RemoveVobFromGlobalVobTree();
		}


		node = node->GetFirstChild();
		while (node != NULL)
		{
			CreateBaseVobTreeNew(node);
			node = node->GetNextChild();
		}
	}


	void SaveCurrentWorldToMSH()
	{
		// ������� ���
		zCWorld* pWorld = ogame->GetWorld();

		// ������� ��� ��������
		oCWorld* oWorld = ogame->GetGameWorld();

		// ���� ���������� ���, �� �������
		if (!pWorld || !oWorld) return;



		zCPolygon::S_ResetMorphedVerts();
		zCVertex::ResetVertexTransforms();

		cmd << Col16(CMD_YELLOW) << "Start removing" << endl;
		zCTree<zCVob>* tree = pWorld->globalVobTree.GetFirstChild();
		while (tree)
		{
			CreateBaseVobTreeNew(tree);
			tree = tree->GetNextChild();
		}

		cmd << Col16(CMD_YELLOW) << "End removing" << endl;
		// ����� ����, ������� ����� �����������
		zCMesh* pMesh = zNEW(zCMesh);

		// ������������ ���-�� ��������� � ����� ����� ������������ ����
		int nPolyMax = 20000;

		// �������� ������ ��� ��������
		pMesh->AllocPolys(nPolyMax);

		// �������� ������ ��� ��������
		pMesh->AllocVerts(nPolyMax * 3);

		// ������ ���������� � ��� ���������
		int nPoly = 0;

		// ���������� ����� ������������ ����
		int nSaved = 1;


		// ����������� �� ���� �����(��� ��������� ������)
		for (int i = 0; i < pWorld->bspTree.numLeafs; i++)
		{
			// ������ ����������� �� ���� ��������� � ������� ���� (��� ��������)
			for (int j = 0; j < pWorld->bspTree.leafList[i].numPolys; j++)
			{

				// ���� �������
				zCPolygon* pPoly = pWorld->bspTree.leafList[i].polyList[j];

				// ���� ��������� � �������
				if (pPoly)
				{
					// ��������� 3 �������� � ����������� ���, ���� ���������� �� �������� �������� ����
					// (�������� ��������� �� ����������� ��������)
					zCVertex* vert1 = pMesh->AddVertex(pPoly->vertex[0]->position);
					zCVertex* vert2 = pMesh->AddVertex(pPoly->vertex[1]->position);
					zCVertex* vert3 = pMesh->AddVertex(pPoly->vertex[2]->position);

					// ��������� ������� � ���, ��������� ����� ���������� ��������� �� ��������,
					// ����� ��������� �������� � ������� ��������
					// (�������� ��������� �� ����������� �������)
					zCPolygon* poly = pMesh->AddPoly(vert1, vert2, vert3);

					// ��������� � �������� ����� �� �������� ��� � � ��������, ������� �� ����
					poly->material = pPoly->material;

					// ��������� ������� ����������� ���������
					// ���� ����� �� ������� ������������� ���-��
					// ����������� ��������� � ���
					if (nPoly >= nPolyMax)
					{
						// ��������� �������� ����������� ����� ����
						string saveName = A "_" + A oWorld->worldName + A "_PART" + A nSaved + A ".MSH";

						// ��������� ���
						pMesh->SaveMSH(saveName.ToChar());

						// ������� ���� � �������
						cmd << Col16(CMD_YELLOW) << saveName << Col16(CMD_WHITE) << " saved" << endl;

						// ����������� ������� ���������� ������
						nSaved++;

						// ���������� �������
						nPoly = 0;

						// ������ ����� ���. �� �� ����� �������� ������ ������!
						// ������� ������� ������ - ���� �� ����.
						pMesh = zNEW(zCMesh);
					}

					// ����������� ������� ���������
					nPoly++;
				}
			}
		}

		// ���� ���� ����������, �� �������� ������������ ��������, ��
		// �� ��������� ������� ���� � ��������� ����
		if (nPoly > 1)
		{
			// ��������� �������� ����������� ����� ����
			string saveName = A "_" + A oWorld->worldName + A "_PART" + A nSaved + A ".MSH";

			// ��������� ���
			pMesh->SaveMSH(saveName.ToChar());

			// ������� ���� � �������
			cmd << Col16(CMD_YELLOW) << saveName << Col16(CMD_WHITE) << " saved" << endl << "\nIt's all!" << endl;
		}
	}




	void CreateWaypointVobTree(zCTree<zCVob>* node)
	{
		zCVob* vob = node->GetData();

		static AddGlobalEntryPointer addEntry = NULL;

		if (!addEntry)
		{
			addEntry = (AddGlobalEntryPointer)GetProcAddress(theApp.module, "AddGlobalEntry");
		}


		if (dynamic_cast<zCVobWaypoint*>(vob))
		{
			Stack_PushString(vob->_GetClassDef()->className);
			Stack_PushString(GetVobName(vob));

			addEntry((uint)vob, vob->GetParent());
		}

		node = node->GetFirstChild();
		while (node != NULL)
		{
			CreateWaypointVobTree(node);
			node = node->GetNextChild();
		}
	}

	void SpacerApp::RebuiltWaypoints()
	{
		zCWorld* world = ogame->GetWorld();

		(callVoidFunc)GetProcAddress(theApp.module, "ClearWaypoints")();


		zCTree<zCVob>* tree = world->globalVobTree.GetFirstChild();
		while (tree)
		{
			CreateWaypointVobTree(tree);
			tree = tree->GetNextChild();
		}

		(callVoidFunc)GetProcAddress(theApp.module, "ReloadWaypoint")();
	}
}