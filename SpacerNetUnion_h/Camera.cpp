// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	struct CameraMovement
	{
		// ���� �������� ������
		float yaw;
		float pitch;
		// ���������� ���� �� ������� ����
		POINT mousePrev;
		// ������� �� ������
		bool hideCursor;

		int vid_x;
		int vid_y;

		CameraMovement::CameraMovement()
		{
			yaw = 90.0f;
			pitch = 0.0f;
			hideCursor = false;

			vid_x = -1;
			vid_y = -1;
		}

	} camMov;


	// ����� ����� ���� �� "�������" ������, ������� ������������� �� � �����, ���� ����

	bool IsSetMouseCenter(POINT& mouse)
	{
		bool flag = false;

		int x = camMov.vid_x;
		int y = camMov.vid_y;

		if (mouse.x > x * 75 / 100)
		{
			mouse.x = x / 2;
			flag = true;
		}

		if (mouse.x < 200)
		{
			mouse.x = x / 2;
			flag = true;
		}

		if (mouse.y < 200)
		{
			mouse.y = y / 2;
			flag = true;
		}

		if (mouse.y > y * 75 / 100)
		{

			mouse.y = y / 2;
			flag = true;
		}

		return flag;
	}


	void CameraMoving()
	{
		if (!ogame->GetCamera())
		{
			return;
		}

		zCVob* movvob = ogame->GetCamera()->connectedVob;




		POINT mouse;
		POINT delta;

		GetCursorPos(&mouse);


		if (zinput->GetMouseButtonPressedRight())
		{

			//	CRect rc;
			//CMainFrame::mainframe->GetWindowRect(rc);

			if (camMov.vid_x == -1)
			{
				camMov.vid_x = zoptions->ReadInt("VIDEO", "zVidResFullscreenX", 1920);
				camMov.vid_y = zoptions->ReadInt("VIDEO", "zVidResFullscreenY", 1080);
			}

			if (IsSetMouseCenter(mouse))
			{
				SetCursorPos(mouse.x, mouse.y);
				camMov.mousePrev = mouse;
			}


			if (!camMov.hideCursor)
			{
				camMov.hideCursor = true;
				camMov.mousePrev = mouse;
				while (ShowCursor(FALSE) >= 0);


				if (theApp.options.GetIntVal("hideCamWindows"))
				{
					(voidFuncPointer)GetProcAddress(theApp.module, "HideWindows")();
				}



			}
		}
		else
		{
			if (camMov.hideCursor)
			{
				camMov.hideCursor = false;
				ShowCursor(TRUE);

				if (!theApp.hideWindows)
				{
					(voidFuncPointer)GetProcAddress(theApp.module, "ShowWindows")();
				}

			}

			return;
		}

		zVEC3 unit = movvob->GetAtVectorWorld();
		zVEC3 pos = movvob->GetPositionWorld();
		zVEC3 newVec = zVEC3(0, 0, 0);


		/*************************** �������������� �������� ������ *****************************/
		float speedTranslate = ((float)(theApp.options.GetIntVal("camTransSpeed")) / 100) * ztimer->frameTimeFloat;


		if (keys.KeyPressed("CAMERA_TRANS_FORWARD", false, true))
		{
			newVec += unit.Normalize() * speedTranslate;
		}


		if (keys.KeyPressed("CAMERA_TRANS_BACKWARD", false, true))
		{
			newVec += (unit.Normalize() * speedTranslate).Minus();
		}

		if (keys.KeyPressed("CAMERA_TRANS_LEFT", false, true))
		{
			newVec += unit.Normalize().Cross(zVEC3(0, 1, 0) * speedTranslate);
		}

		if (keys.KeyPressed("CAMERA_TRANS_RIGHT", false, true))
		{
			newVec += unit.Normalize().Cross(zVEC3(0, -1, 0) * speedTranslate);
		}


		if (keys.KeyPressed("CAMERA_TRANS_UP", false, true))
		{

			newVec += zVEC3(0, 1, 0) * speedTranslate;
		}

		if (keys.KeyPressed("CAMERA_TRANS_DOWN", false, true))
		{
			newVec += zVEC3(0, -1, 0) * speedTranslate;
		}


		// �������� � n-��� ��� ������� ����� �����
		if (keys.KeyPressed("CAM_SPEED_X10", false, true))
		{
			newVec *= 10;
		}

		// ��������� � 5 ��� ��� ������� ����� ��������
		if (keys.KeyPressed("CAM_SPEED_MINUS_10", false, true))
		{
			newVec *= 0.2;
		}

		// ����� ������� ����������� �� ����� � ����������� �������
		if (KeyClick(MOUSE_WHEELUP))
		{
			newVec = unit * 2000;
		}

		if (KeyClick(MOUSE_WHEELDOWN))
		{
			newVec = unit.Minus() * 2000;
		}

		// ���������� ������� � ����
		movvob->SetPositionWorld(pos + newVec);


		/*************************** ������������ �������� ������ *****************************/

		// ��������� ��������� ����
		delta.x = (mouse.x - camMov.mousePrev.x);
		delta.y = (camMov.mousePrev.y - mouse.y);

		camMov.mousePrev = mouse;

		float rotSpeed = (float)(theApp.options.GetIntVal("camRotSpeed")) / 100;
		// ���� ��������
		camMov.yaw -= delta.x * rotSpeed;
		camMov.pitch += delta.y * rotSpeed;

		// ������������ ���� ���� ������, ����� ��� �� ��������� �� 360 �������� �� ��������� � ��� �� ���������������
		if (camMov.pitch > 89.0f)
			camMov.pitch = 89.0f;
		if (camMov.pitch < -89.0f)
			camMov.pitch = -89.0f;


		// ������ �������� � ������������
		zVEC3 rot;

		rot.n[0] = cos(Radian(camMov.yaw)) * cos(Radian(camMov.pitch));
		rot.n[1] = sin(Radian(camMov.pitch));
		rot.n[2] = sin(Radian(camMov.yaw)) * cos(Radian(camMov.pitch));

		movvob->SetHeadingAtWorld(rot.Normalize());
	}
}