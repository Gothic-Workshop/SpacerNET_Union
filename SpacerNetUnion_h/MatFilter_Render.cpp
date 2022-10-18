// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	zCArray<MatFilterRenderEntry*> pListCache;
	//zCTextureConvert* texConv = NULL;

	void RenderTexture_Send(MatFilterRenderEntry* entry)
	{
		if (entry)
		{
			//cmd << "Send: " << entry->name << endl;

			DWORD* addr = &entry->pixels[0];
			int size = 128 * 128;
			uint addrSend = (uint)addr;


			Stack_PushUInt(addrSend);
			Stack_PushInt(size);

			//cmd << "size: " << size << " addr: " << addrSend << endl;


			//cmd << "first: " << entry->pixels[0] << endl;

			theApp.exports.MatFilter_SendTexture();
		}
		
	}
	void RenderTextureByPixels3(const zSTRING& texName, int posX, int posY)
	{

		cmd << "RenderTextureByPixels3: try texture: " << texName << endl;
		// Direct3D ������ ��� �2�
		IDirect3DDevice7*& dev = *(IDirect3DDevice7**)0x009FC9F4;

		//cmd << "RenderTextureByPixels3: " << pListCache.GetNumInList() << endl;

		

		for (int i = 0; i < pListCache.GetNumInList(); i++)
		{
			if (auto entry = pListCache.GetSafe(i))
			{
				if (entry->name == texName)
				{
					//cmd << "Texture " << texName << " found" << endl;
					RenderTexture_Send(entry);
					return;
				}
			}
		}


		RX_Begin(3);
		// ������ ���������� ���������� - ���������� � ���������� (�� ���� �� ������� ��� �����!)
		// ������ ���-�� �������� ����� � ������������
		zoptions->ChangeDir(DIR_TEX);


		zCTextureConvert* texConv = zrenderer->CreateTextureConvert();
		

		// ������ ��������� TGA-������ (�������� � TEX ����)
		zCTextureFileFormatTGA*    texFile = new zCTextureFileFormatTGA();

		// ��������� �������� � �������� � � ������� ��������
		texFile->LoadTexture(texName, texConv);

		// ����������� �� ���������� ��������, �� ��� ������ �� �����
		zRELEASE(texFile);

		// ���� ���������� � ��������, ������������� � ������ �������-���������
		zCTextureInfo texInfo = texConv->GetTextureInfo();

		///texConv->SetTextureInfo(texInfo);

		// ������������ �������� � ������ ������:
		// ������������� ����� ������ ������������ �������� ��������
		texInfo.texFormat = zRND_TEX_FORMAT_RGBA_8888;

		// � ����� ������� �������� (����� ��������� � ������� ������):
		texInfo.sizeX = 128; // ������
		texInfo.sizeY = 128; // ������
							 // ������������ �������� ������ ��������� � �������� ��������� ��������
		zBOOL result = texConv->ConvertTextureFormat(texInfo);

		if (!result)
		{
			cmd << "Can't convert!" << endl;
			return;
		}
		// ���� ��������� ��������, �� ��������� �������. ���� ���, �� �������.
		//***************************
		// ��������� ����� �� ������
		//***************************
		

		// ��������� ���� �������
		D3DCOLOR COL;

		// ��������� RGBA �������
		zVEC4 col;

		auto entry = new MatFilterRenderEntry();
		entry->name = texName;
		//entry->pixels.AllocAbs(128 * 128);

		pListCache.InsertEnd(entry);
		int count = 0;

		// �� �������������� �������� �������� (� ������ ������ 128)
		for (int x = 0; x < texInfo.sizeX; x++)
		{
			// �� ������������ �������� �������� (� ������ ������ ���� 128)
			for (int y = 0; y < texInfo.sizeY; y++)
			{
				// �������� RGBA ���� ����� �� �����������
				col = texConv->GetRGBAAtTexel(x, y);

				// ���� ����� � ���� DWORD �����
				// ������ � ����� ���� ��� ��� ������� Clear().
				COL = RGBA(col[0], col[1], col[2], col[3]);


				//COL = RGBA2DWORD(col[0], col[1], col[2], col[3]);
				
				//entry->pixels.InsertEnd(RGBA2DWORD(col[0], col[1], col[2], col[3]));

				entry->pixels[count] = COL;
				// posX - ������ ��������� �� ��� �(� ��������)
				// posY - ������ ��������� �� ��� y(� ��������)
				// x - ������� ������� �� ��� � �� ��������, � - �������������� �� ��� Y
				// ������������� � ���� ����� ��� ������������ ������������
				//D3DRECT rec = { posX + x, posY + y, posX + x + 1, posY + y + 1 };

				// ����������� ������ � ������� ������� ������� (����� ������� ������ �����)
				//dev->Clear(1, &rec, D3DCLEAR_TARGET, COL, 1.0f, 0);

				

				
				count += 1;
			}
		}

		//cmd << "Count: Image " << count << endl;
		RX_End(3);

		cmd << texName << " convert: " << RX_PerfString(3) << endl;

		

		RenderTexture_Send(entry);

		

		delete texConv;
	}

	void MatFilter::Render(zCMaterial* pMat)
	{
		// ���������� ������ (�����������)
		int vposX = 0;
		int vposY = 0;

		int offset = 128; // ���� �� ������������
		static int lastMat = 0; // ��� ��������� ����� ��������� �� ������ ������ ����������, ������� ��� ������� �� �����

								// ���� � ���� ��� ��������
		if (!pMat || !pMat->texture)
			// �������
			return;

		// ���� �������� �������� ���������
		zSTRING texName = pMat->texture->GetObjectName();

		//texName = "RED.tga";

		// ���������� ������ ����������� ������� � �������
		RenderTextureByPixels3(texName, screen->nax(vposX), screen->nay(vposY));

		
	}
}