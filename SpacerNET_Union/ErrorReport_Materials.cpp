// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	void ErrorReport::CheckPolygons()
	{
		zCPolygon**& trisList = ogame->GetWorld()->bspTree.treePolyList;
		zCArray<zCMaterial*> mats;
		zCArray<zCMaterial*> matsChecked;

		int numPolys = ogame->GetWorld()->bspTree.numPolys;

		for (int i = 0; i < numPolys; i++)
		{
			zCPolygon* poly = trisList[i];
			if (poly->flags.ghostOccluder != 0) continue;

			auto mat = poly->material;

			if (mat && mat->texture)
			{
				if (!matsChecked.IsInList(mat))
				{
					matsChecked.InsertEnd(mat);

					auto searchName = mat->texture->GetObjectName();

					bool foundVirtual = false;

					auto result = vdf_fexists(searchName.ToChar(), VDF_VIRTUAL);

					if ((result & VDF_VIRTUAL) == VDF_VIRTUAL)
					{
						foundVirtual = true;
					}


					result = vdf_fexists(searchName.ToChar(), VDF_PHYSICAL);

					if ((result & VDF_PHYSICAL) == VDF_PHYSICAL)
					{
						if (foundVirtual)
						{

						}
						else
						{

						}
					}


					if (result == 0 && !foundVirtual)
					{
						if (!mats.IsInList(mat))
						{
							mats.InsertEnd(mat);

							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_MESH_MAT_TEXTURE_NOT_FOUND);
							entry->SetObject((uint)mat);
							entry->SetVobName(mat->GetName());
							entry->SetMaterialName(mat->GetName());
							entry->SetTextureName(mat->texture->GetObjectName());

							AddEntry(entry);

						}
					}

					if (mat->GetObjectName().contains(".0"))
					{
						if (!mats.IsInList(mat))
						{
							mats.InsertEnd(mat);

							auto entry = new ErrorReportEntry();

							entry->SetErrorType(ERROR_REPORT_TYPE_INFO);
							entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_MESH_MAT_NAME);
							entry->SetObject((uint)mat);
							entry->SetVobName(mat->GetName());
							entry->SetMaterialName(mat->GetName());
							entry->SetTextureName(mat->texture->GetObjectName());

							AddEntry(entry);
						}
					}

					if (mat->texture->GetObjectName().contains(".TGA.TGA") || mat->texture->GetObjectName().contains(".TGA.0"))
					{
						mats.InsertEnd(mat);

						auto entry = new ErrorReportEntry();

						entry->SetErrorType(ERROR_REPORT_TYPE_CRITICAL);
						entry->SetProblemType(ERROR_REPORT_PROBLEM_TYPE_MESH_MAT_TEXTURE_BAD_NAME);
						entry->SetObject((uint)mat);
						entry->SetVobName(mat->GetName());
						entry->SetMaterialName(mat->GetName());
						entry->SetTextureName(mat->texture->GetObjectName());

						AddEntry(entry);
					}


				}

			}
		}
	}

}