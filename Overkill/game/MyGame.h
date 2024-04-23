#pragma once


#include "Game.h"


class CMyGame : public CGame
{
public:
	CMyGame();
	~CMyGame();

    // ----  Declare your game variables and objects here -------------

	// Variables
	int score;
	
	int reserve;
	int ammo;
	int iframes;
	int bulletsfired;
	bool parry;
	int parrycooldown;
	int firerate;
	int reload;
	int wave;
	bool bossready;
	bool bossspawned;
	bool bosskilled;
	int bosseskilled;
	bool isendless;
	bool isaiming;
	bool isgamewon;
	bool endless;
	bool iscontrols;
	bool ismodeselect;
    // Declare Sprites
    CSprite startScreen;
	CSprite crosshair;
	CSprite endScreen;
	CSprite startButton;
	CSprite quitButton;
	CSprite menuButton;
	CSprite controlsButton;
	CSprite controlsScreen;
	CSprite endlessButton;
	CSprite Modeselectscreen;
	CSprite normalButton;
	CSprite resumeButton;

    // Declare Models
    CModel house;
    CModelMd2 player;     // animated model
    CModelMd2 enemy;// enemy md2 model
	CModelMd2 enemy2;
	CModelMd2 enemy3;
	CModel box;
	CModelMd2 bossbullet;
	CModelMd2 boss;
	CModelMd2 boss2;
	CModelMd2 boss3;
	
	//  Declare Model Lists
	CModelList enemies;   // list containing enemy models
	CModelList enemies2;
	CModelList enemies3;
	CModelList outerWalls;  // list containing outer walls
	CModelList shots; // list containing shots
	CModelList enemyshots;
	CModelList boxes;
	CModelList bosses;
	CModelList bosses2;
	CModelList bosses3;
	CModelList bossbullets;

	// game world floor
	CFloor floor;
	// sound 
	CSoundPlayer shot;
	CSoundPlayer hurt;
	CSoundPlayer death;
	CSoundPlayer reloadsound;
	CSoundPlayer bossdeath; 
	CSoundPlayer bossdeath2;
	CSoundPlayer bossdeath3;
	CSoundPlayer bosshit;
	CSoundPlayer parrysound;
	CSoundPlayer ogrodeath;
	CSoundPlayer aliendeath;
	CSoundPlayer goblindeath;
	CSoundPlayer maintheme;
	CSoundPlayer bosstheme;
	CSoundPlayer normaltheme;
	CSoundPlayer select;
	CSoundPlayer exit;
	CSoundPlayer pause;
	CSoundPlayer unpause;
	// Font
	CFont font;
	
	// health indicator
	CHealthBar housebar;
	CHealthBar playerbar;
	CHealthBar Waveprogression;
	CHealthBar bossbar;
	CHealthBar bossbar2;
	CHealthBar bossbar3;
	 // -----   Add you member functions here ------
   
   void PlayerControl();
   void EnemyControl(); 
   void ShotsControl();
   void Wavecontrol();
   void boxcontrol();
   void bosscontrol();
   
   void CameraControl(CGraphics* g);
	
   // ---------------------Event Handling --------------------------

	// Game Loop Funtions
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);
	virtual void OnRender3D(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartLevel(int level);
	virtual void OnStartGame();
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y);
};
