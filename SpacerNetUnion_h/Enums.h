// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// Add your code here . . .
	enum CompileLightMod
	{
		VertexOnly,
		Low,
		Mid,
		High
	};


	enum zSAVE_TYPE { SAVE_ZEN, SAVE_ZEN_UC, SAVE_ZEN_BIN, SAVE_3DS };
	enum zLOAD_TYPE { LOAD_ZEN, LOAD_ZEN_UC, LOAD_3DS };


	enum TPropEditType {
		PETunknown,
		PETbool,
		PETenum,
		PETint,
		PETfloat,
		PETstring,
		PETvec3,
		PETcolor,
		PETraw,
		PETclass,
		PETgroup,
		PETchunkend
	};


	enum zTWorldLoadMergeMode {
		zWLD_LOAD_MERGE_ADD,
		zWLD_LOAD_MERGE_REPLACE_ROOT_VISUAL
	};


	enum TurnWpMode {
		NONE = 0,
		AGAINST_CAMERA,
		ON_CAMERA
	};

	enum SpacerWorkMode 
	{
		SWM_VOBS = 0,
		SWM_MATERIALS = 1,
		SWM_GRASS = 2
	};


	enum ToggleMenuType
	{
		ToggleVobs = 0,
		ToggleWaynet,
		ToggleHelpers,
		ToggleBbox,
		ToggleInvis
	};

	enum SearchVobType
	{
		Search = 0,
		Convert,
		ReplaceVobTree,
		Remove,
		Rename
	};

	enum zEVobMotionMode {
		zMM_NONE,
		zMM_MOVE,
		zMM_ROTATE,
		zMM_SCALE
	};

	enum zEVobMotionHeading {
		zMH_WORLD,
		zMH_LOCALE,
		zMH_VIEW
	};

	enum zEVobCollisionMode {
		zVC_DEFAULT,
		zVC_DISABLED,
		zVC_ENABLED
	};

	enum zECamMotionMode {
		zCM_DISABLED,
		zCM_ENABLED
	};

	enum zEPickMode {
		zPM_DISABLED,
		zPM_VOBS,
		zPM_POLYS
	};

	enum zEPickMethod {
		zPM_CURSOR,
		zPM_CROSSHAIR
	};

	struct zTWE_ControllerEvents {
		Array<zCVob*>      SelectedVobs;
		zCVob*             PickedVob;
		zCPolygon*         PickedPolygon;
		zEPickMode         PickMode;
		zEPickMethod       PickMethod;
		zEVobMotionMode    MotionMode;
		zEVobMotionHeading MotionHeading;
		zEVobCollisionMode CollisionMode;
		zECamMotionMode    CameraMoving;
		zVEC2              CursorPosition;
		float              GizmoScale;
	};

	
}

DllExport Gothic_II_Addon::zTWE_ControllerEvents ControllerEvents;
