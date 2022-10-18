// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {

	// Add your code here . . .
	void MatFilter::Init()
	{
		this->filterMatBlocked = false;

		this->LoadFilterList();

		this->init = true;
	}


	void MatFilter::ToggleWindow(bool value)
	{
		Stack_PushInt(filterMatBlocked);
		Stack_PushInt(value);
		theApp.exports.MatFilter_ToggleWindow();
	}

	void MatFilter::Reset()
	{
		ToggleWindow(false);
	}



	void MatFilter::FillInterfaceData()
	{

		ToggleWindow(!filterMatBlocked);

		if (filterMatBlocked)
		{
			return;
		}



		theApp.exports.Clear_MatFilter_Filters();

		for (int i = 0; i < matFilterList.GetNum(); i++)
		{
			auto entry = matFilterList.GetSafe(i);

			if (entry)
			{
				//cmd << "Push " << entry->name << endl;

				Stack_PushString(entry->name);
				Stack_PushInt(entry->id);
				theApp.exports.Fill_MatFilter_Filters();
			}
		}
	}


	int MatFilter::FindMatFilterIndexByName(CString filterName)
	{
		for (int i = 0; i < matFilterList.GetNum(); i++)
		{
			auto entry = matFilterList.GetSafe(i);

			if (entry)
			{
				CString checkName = entry->name;

				checkName.Lower();

				if (checkName == filterName.Lower())
				{
					return i;
				}

			}
		}

		return -1;
	}





	void MatFilter::OnSelectInMatFilter(zCMaterial* mat)
	{
		int index = mat->libFlag;

		if (index >= 0)
		{
			Stack_PushInt(index);
			theApp.exports.MatFilter_SelectFilterByIndex();

			Stack_PushUInt((uint)mat);
			theApp.exports.MatFilter_SelectMaterialByAddr();



		}
	}

	void MatFilter::FillMatListByFilterName(CString filter)
	{

		cmd << "Using filter: " << filter << endl;

		zCArray<zCMaterial*>	pList;

		spcCMatFilter* trashItem = 0;
		spcCMatFilter* filterItem = 0;

		int pos = FindMatFilterIndexByName(filter);
		if (pos >= 0)		 filterItem = matFilterList[pos];

		if (!filterItem) return;

		//cmd << "pos: " << pos << endl;

		int undefPos = FindMatFilterIndexByName(FILTER_NAME_TRASH);
		if (undefPos >= 0)
		{
			trashItem = matFilterList[undefPos];
		}
		else
		{
			cmd << "No filter with id: " << undefPos << " Name: " << filter << endl;
			return;
		}

		zCClassDef* classDef = zCMaterial::classDef;

		zCMaterial*	mat = 0;
		unsigned int	matIter = 0;
		unsigned int	numOfMats = classDef->objectList.GetNum();

		if (numOfMats>matIter) mat = dynamic_cast<zCMaterial*>(classDef->objectList[matIter]);

		cmd << "filterItem: " << filterItem->name << " id: " << filterItem->id << endl;

		//cmd << "numOfMats: " << numOfMats << endl;
		// iterate materials in matlist
		if (pos >= 0)
		{
			while (mat)
			{
				// zCTexture* texture = mat->GetTexture();
				// if (texture) texture->SetCacheOutLock(TRUE);
				// So, nun feststellen, ob das Material wirklich zu dem Filter gehoert


				if (mat->GetName().Length() > 0 && (mat->matUsage >= zCMaterial::zMAT_USAGE_LEVEL) && (mat->libFlag == filterItem->id))
				{

					pList.Insert(mat);
					/*
					// insert to current-list
					lList = &current_matlist;
					if (lList)
					{
					// suche nach einfuegepunkt (sortiert)
					do
					{
					nextMat = 0;
					nextList = lList->GetNextInList();
					if (nextList)
					{
					nextMat = nextList->GetData();
					if (mat->GetName() < nextMat->GetName()) break;
					lList = nextList;
					}
					} while (nextList);
					if (lList) lList->Insert(mat);
					}
					else
					{
					current_matlist.Insert(mat);
					}
					*/
				}

				matIter++;
				if (numOfMats>matIter) mat = dynamic_cast<zCMaterial*>(classDef->objectList[matIter]);
				else mat = 0;
			}
		}
		cmd << "pList: " << pList.GetNum() << endl;
		for (int i = 0; i < pList.GetNum(); i++)
		{
			auto entry = pList.GetSafe(i);

			if (entry)
			{
				Stack_PushString(entry->GetName());
				Stack_PushUInt((uint)entry);
				theApp.exports.AddMatByMatFilterName();
			}

		}
		//AddMatByMatFilterName
	}

	void MatFilter::AddNewFilter(CString name)
	{
		spcCMatFilter* item = new spcCMatFilter;
		//item->Init(FILTER_NAME_TRASH, TMatLibFlag::NullLib);
		matFilterList.Insert(item);

		item->name = name;
		item->id = matFilterList.GetNum() - 1;

		cmd << "Add new filter " << name << " with id " << item->id << endl;
		//item->id = matFilterList.GetNum() - 1;




	}
	void MatFilter::OnRenameFilter(int index, CString name)
	{

		if (index >= 0)
		{
			auto entry = matFilterList.GetSafe(index);

			if (entry)
			{
				entry->name = name;
			}
		}

	}
}