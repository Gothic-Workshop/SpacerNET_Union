// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	zCArray<MatFilterRenderEntry*> pListCache;
	const int OUTPUT_SIZE = 128;


	// ������ �� ������ �� ��������
	void RenderOnScreen(int x, int y, D3DCOLOR COL)
	{
		static IDirect3DDevice7*& dev = *(IDirect3DDevice7**)0x009FC9F4;

		D3DRECT rec = { 500 + x, 400 + y, 500 + x + 1, 400 + y + 1 };

		dev->Clear(1, &rec, D3DCLEAR_TARGET, COL, 1.0f, 0);
	}


	// �������� �������� � ���������
	void RenderTexture_Send(MatFilterRenderEntry* entry)
	{
		if (entry)
		{
			//cmd << "Send: " << entry->name << endl;

			DWORD* addr = &entry->pixels[0];
			int size = OUTPUT_SIZE * OUTPUT_SIZE;
			uint addrSend = (uint)addr;

			for (int x = 0; x < OUTPUT_SIZE; x++)
			{
				for (int y = 0; y < OUTPUT_SIZE; y++)
				{
					int pos = x * OUTPUT_SIZE + y;

					auto COL = entry->pixels[pos];

					RenderOnScreen(x, y, COL);
				}
			}

			Stack_PushUInt(addrSend);
			Stack_PushInt(size);

			theApp.exports.MatFilter_SendTexture();
		}
		
	}

	

	void RenderTextureByPixels3(const zSTRING& texName)
	{

		//cmd << "RenderTextureByPixels3: try texture: " << texName << endl;
		// Direct3D ������ ��� �2�
		

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
		//zoptions->ChangeDir(DIR_TEX);


		zCTextureConvert* texConv = zrenderer->CreateTextureConvert();

		// ������ ���������
		if (!texConv)
			return;

		if (!texConv->LoadFromFileFormat(Z texName))
		{
			// ���� �� ���������, �� ������� ���������
			cmd << "Texture '" << texName << "' not load!";

			// ������� ��������
			delete texConv;

			// �������
			return;
		}
		
		// �� ��������� ������� ������ �������� ����� ��� ����� �������
		BOOL bUseOriginalColor = TRUE;

		//**************
		// �����������
		//**************
		// ���� ���������� � ��������, ������������� � ������ �������-���������
		zCTextureInfo texInfo = texConv->GetTextureInfo();

		cmd << "Original: " << texInfo.texFormat << endl;
		
		zTRnd_TextureFormat originalFormat = texInfo.texFormat;

		// ������������ �������� � ������ ������:
		// ������������� ����� ������ ������������ �������� ��������
		texInfo.texFormat = zRND_TEX_FORMAT_RGBA_8888;

		texInfo.numMipMap = 1;

		int result = texConv->ConvertTextureFormat(texInfo);


		// ���� ����������� �������� ������ �������
		// � � ������� ���������� �� 128�128
		if (result && texInfo.sizeX != OUTPUT_SIZE && texInfo.sizeY != OUTPUT_SIZE)
		{
			// ��������� ��������� ���������������:
			// ����� ������� �������� (����� ��������� � ������� ������)
			texInfo.sizeX = OUTPUT_SIZE; // ������
			texInfo.sizeY = OUTPUT_SIZE; // ������

								 // ��������� ����������� ��������
			result = texConv->ConvertTextureFormat(texInfo);

			// ���������� ����� �������� ����� RGBA ��� ��������
			bUseOriginalColor = FALSE;
		}

							 // ������������ �������� ������ ��������� � �������� ��������� ��������
		

		// ���� ��������� ��������, �� ��������� �������
		// ���� ���, �� �������
		if (!result)
		{
			// ������� ��������
			delete texConv;
			cmd << "Can't convert! " << texName << endl;
			// �������
			return;
		}


		// ��������� ���� �������
		D3DCOLOR COL;

		// ��������� RGBA �������
		zVEC4 col;

		auto entry = new MatFilterRenderEntry();
		entry->name = texName;

		pListCache.InsertEnd(entry);
		int count = 0;

		cmd << "Format: " << texInfo.texFormat << endl;

		// �� �������������� �������� �������� (� ������ ������ 128)
		for (int x = 0; x < texInfo.sizeX; x++)
		{
			// �� ������������ �������� �������� (� ������ ������ ���� 128)
			for (int y = 0; y < texInfo.sizeY; y++)
			{
				// �������� RGBA ���� ����� �� �����������
				col = texConv->GetRGBAAtTexel(x, y);


				// �������� ��� �����, �� ��������� �� ������
				if (originalFormat == zRND_TEX_FORMAT_DXT1)
				{
					col.n[3] = 255.0f;
				}
				//cmd << "R: " << col.n[0] << " G: " << col.n[1] << " B: " << col.n[2] << " A: " << col.n[3] << endl;

				if (bUseOriginalColor)
					COL = RGBA(col[0], col[1], col[2], col[3]); // ������
				else
					COL = RGBA(col[2], col[1], col[0], col[3]); // �����


				
				entry->pixels[count] = COL;
				// posX - ������ ��������� �� ��� �(� ��������)
				// posY - ������ ��������� �� ��� y(� ��������)
				// x - ������� ������� �� ��� � �� ��������, � - �������������� �� ��� Y
				// ������������� � ���� ����� ��� ������������ ������������

				RenderOnScreen(x, y, COL);
				
				

				
				count += 1;
			}
		}
		
		for (int i = 0; i < OUTPUT_SIZE * OUTPUT_SIZE; i++)
		{
			RGBA col = DWORD2RGBA(entry->pixels[i]);

			//cmd << "R: " << col.R << " G: " << col.G << " B: " << col.B << " A: " << col.Alpha  << endl;
		}
		//cmd << "Count: Image " << count << endl;
		RX_End(3);

		cmd << texName << " convert: " << RX_PerfString(3) << " originalColor: " << bUseOriginalColor << endl;

		

		RenderTexture_Send(entry);

		

		delete texConv;
	}


	static zSTRING lastName = "";

	void MatFilter::Loop()
	{
	
		if (lastName != "")
		{
			//RenderTextureByPixels3(Z lastName);
		}
	}
	void MatFilter::Render(zCMaterial* pMat)
	{
		if (!pMat || !pMat->texture)
			// �������
			return;


		// �������� ������ �������� � ���������
		int x, y;
		pMat->texture->GetPixelSize(x, y);
		CString sizeStr = Z x + "x" + Z y;


		Stack_PushString(pMat->texture->GetObjectName());
		Stack_PushString(sizeStr);
		theApp.exports.MatFilter_UpdateTextureSize();
	





		zSTRING cTEX = "-C.TEX";
		// ���� �������� �������� ���������
		zSTRING texName = pMat->texture->GetObjectName();

		//texName = "roof_batons_alpha.TGA";

		texName.Upper();

		cmd << "Input: " << texName << endl;

		zSTRING originalName = texName;

		originalName = originalName.Cut(originalName.Length() - 4, 4);
		//cmd << "originalName: " << originalName << endl;





		// ��������� ��������� ��� ���������������� �������
		
		zSTRING innerFormatName = originalName + cTEX;

		auto result = vdf_fexists(innerFormatName.ToChar(), VDF_DEFAULT);
		bool texFound = false;

		if ((result & VDF_VIRTUAL) == VDF_VIRTUAL)
		{
			cmd << innerFormatName << " TEX found in VDF" << endl;
			texFound = true;
		}

		if ((result & VDF_PHYSICAL) == VDF_PHYSICAL)
		{
			cmd << innerFormatName << " TEX found in _WORK" << endl;
			texFound = true;
		}

		if (texFound)
		{
			lastName = innerFormatName;
			RenderTextureByPixels3(innerFormatName);
		}
		else
		{
			auto result = vdf_fexists(texName.ToChar(), VDF_DEFAULT);
			bool TGAFound = false;

			if ((result & VDF_VIRTUAL) == VDF_VIRTUAL)
			{
				cmd << texName << " TGA found in VDF" << endl;
				TGAFound = true;
			}

			if ((result & VDF_PHYSICAL) == VDF_PHYSICAL)
			{
				cmd << texName << " TGA found in _WORK" << endl;
				TGAFound = true;
			}

			if (TGAFound)
			{
				RenderTextureByPixels3(texName);
			}
		}


	}
}