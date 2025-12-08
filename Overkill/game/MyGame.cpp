#include "MyGame.h"

using namespace std;

CMyGame::CMyGame(void) {}

CMyGame::~CMyGame(void) {}



// --------  game initialisation --------
void CMyGame::OnInitialize()
{
	// Loading graphics and sound assets
	cout << "Loading assets" << endl;
	
	font.LoadDefault(); 
	
	// enable lighting
	Light.Enable();
	
    // house model
    house.LoadModel("house/house.obj"); 
	house.SetScale(0.8f); house.SetPosition(0,0,0);

	// player model
	player.LoadModel("Abarlith/Abarlith.md2"); 
	player.SetScale( 3.5f);
	
	// enemy model 1
	enemy.LoadModel("Ogro/Ogro.md2"); 
	enemy.SetScale( 3.5f); enemy.SetToFloor(0);
	
	// enemy model 2
	enemy2.LoadModel("alien/alien.md2");
	enemy2.SetScale(3.5f); enemy2.SetToFloor(0);
	
	// enemy model 3
	enemy3.LoadModel("goblin/goblin.md2");
	enemy3.SetScale(2.5f); enemy3.SetToFloor(0);
	// boss model
	boss.LoadModel("Alien2/Alien2.md2");
	boss.SetScale(10.0f); boss.SetToFloor(0);
	
	
	// boss 2 model
	boss2.LoadModel("droid/droid.md2");
	boss2.SetScale(10.0f); boss2.SetToFloor(0);
	
	// boss 3 model
	boss3.LoadModel("troll/troll.md2");
	boss3.SetScale(10.0f); boss3.SetToFloor(0);

	// box models
	box.LoadModel("box/box.obj");
	box.SetScale(0.5f); box.SetToFloor(0);
	// floor texture
	floor.LoadTexture("floor.bmp");
	floor.SetTiling(true);
	
	// start screen
	startScreen.LoadImage("startScreen.bmp");
	startScreen.SetPosition((float)Width/2,(float)Height/2);
	
	// crosshair
	crosshair.LoadImage("crosshair.bmp", CColor::Red());
	
	// end screen
	endScreen.LoadImage("endscreen.bmp");
	endScreen.SetPosition((float)Width / 2, (float)Height / 2);
	
	// start button
	startButton.LoadImage("Start.bmp");
	// quit button
	quitButton.LoadImage("quit.bmp");
	// menu button
	menuButton.LoadImage("Menu.bmp");
	// control screen
	controlsScreen.LoadImage("controlscreen.bmp");
	controlsScreen.SetPosition((float)Width /2, (float)Height / 2);
	// control button
	controlsButton.LoadImage("controls.bmp");
	// endless button
	endlessButton.LoadImage("Endless.bmp");
	// mode select screen
	Modeselectscreen.LoadImage("Mode.bmp");
	Modeselectscreen.SetPosition((float)Width / 2, (float)Height / 2);
	// normal button
	normalButton.LoadImage("normal.bmp");
	// resume button
	resumeButton.LoadImage("Resume.bmp");
}


void CMyGame::OnStartLevel(int level)
{
  // set size of the game world
  floor.SetSize(9000,9000);
  
  enemies.delete_all();
  
  shots.delete_all();
  
  // create four outer walls with a dark grey colour
  outerWalls.delete_all();
  
  CModel* pWall = new CModel(0,25,-1500,3000,60,50,CColor(61,61,67));
  outerWalls.push_back(pWall);
  
  pWall = new CModel(0,25, 1500,3000,60,50,CColor(61,61,67));
  outerWalls.push_back(pWall);
  
  pWall = new CModel(1500,25, 0,50,60,3000,CColor(61,61,67));
  outerWalls.push_back(pWall);
  
  pWall = new CModel(-1500,25, 0,50,60,3000,CColor(61,61,67));
  outerWalls.push_back(pWall);
  
  // player position
  player.SetPosition(400,100,300);
  
  // reset score
  score=0; 
  
  // setup healthbar
  housebar.SetSize(100,15);
  
  housebar.SetHealth(100);

  
  playerbar.SetSize(100, 15);
 
  playerbar.SetHealth(100);
  Waveprogression.SetSize(1000, 15);
  Waveprogression.SetHealth(0);
  
  
 
}

// Game Logic in the OnUpdate function called every frame

void CMyGame::OnUpdate() 
{
	if (IsMenuMode() || IsGameOver()) return;
	
	long t =  GetTime();
	
	// --- updating models ----
	player.Update(t);
	enemies.Update(t);
	enemies2.Update(t);
	enemy.Update(t);
	enemy2.Update(t);
	shots.Update(t);
	enemyshots.Update(t);
	bosses.Update(t);
	bosses2.Update(t);
	bosses3.Update(t);

	bossbullets.Update(t);
	enemies3.Update(t);
	for (CModel* pBox : boxes) pBox->Update(t);
    PlayerControl();
    EnemyControl();
    ShotsControl();
	Wavecontrol();
	boxcontrol();
	bosscontrol();
	
	if (housebar.GetHealth() <= 0) 
	{
		death.Play("player_death.wav");
		GameOver();
	}
	if (playerbar.GetHealth() <= 0)
	{
		GameOver();
		death.Play("player_death.wav");
	}
	if (iframes > 0) iframes--;
	if (parrycooldown > 0) parrycooldown--;
	if (firerate > 0) firerate--;
	if (reload > 0) reload--;
}


void CMyGame::PlayerControl()
{
	

	// align direction with rotation
	player.SetDirectionV(player.GetRotationV());

	// -----  player speed --------
	if (IsKeyDown(SDLK_w))
	{ 
		// don't move player forward when colliding with the house
		if (!player.HitTestFront(&house)) player.SetSpeed(400);
		else player.SetSpeed(0);	
	}
	else player.SetSpeed(0);

	if (IsKeyDown(SDLK_s))
	{
		// don't move player forward when colliding with the house
		if (!player.HitTestFront(&house)) player.SetSpeed(-400);
		else player.SetSpeed(0);
	}
	

	// play running sequence when UP key is pressed
	// otherwise play standing sequence
	if (IsKeyDown(SDLK_w))  player.PlayAnimation("run",7,true);
		else if (IsKeyDown(SDLK_s))  player.PlayAnimation("run", 7, true);
	else if (IsKeyUp(SDLK_f))player.PlayAnimation("stand",7,true);
	

    // prevent player from getting across the walls
	for (CModel* wall : outerWalls)
	{
		if (player.HitTestFront(wall)) player.SetSpeed(0);
	}

	
	// collision detection between player and enemies
	for (CModel* pEnemy : enemies)
	{
	 if (player.HitTest( pEnemy))
	 {
		 if (iframes == 0)
		 {
			 if (parry == 0)
			 {
				 playerbar.SetHealth(playerbar.GetHealth() - 10);
				 player.PlayAnimation("pain", 10, false);
				 hurt.Play("hurt.wav");
				 iframes = 15;
				 
			 }
			 if (parry == 1)
			 {
				 player.PlayAnimation("wave", 10, true);
				 parrysound.Play("parry.wav");
				 ogrodeath.Play("ogro_death.wav");
				 pEnemy->Delete();
				 Waveprogression.SetHealth(Waveprogression.GetHealth() + 10);
				 parrycooldown = 120;
				 parry = false;
				 if (reserve < 120)
				 {
					 reserve = reserve + 60;
				 }
				 if (playerbar.GetHealth() != 100)
				 {
					 playerbar.SetHealth(playerbar.GetHealth() + 10);
				 }
			 }
		 }
	 }
	 
    }
	
	for (CModel* pAlien : enemies2)
	{
		if (player.HitTest(pAlien))
		{
			if (iframes == 0)
			{
				if (parry == 0)
				{
					playerbar.SetHealth(playerbar.GetHealth() - 10);
					player.PlayAnimation("pain", 10, false);
					hurt.Play("hurt.wav");
					iframes = 15;

				}
				if (parry == 1)
				{
					player.PlayAnimation("wave", 10, true);
					parrysound.Play("parry.wav");
					aliendeath.Play("alien_death.wav");
					pAlien->Delete();
					Waveprogression.SetHealth(Waveprogression.GetHealth() + 10);
					parrycooldown = 120;
					parry = false;
					if (reserve < 120)
					{
						reserve = reserve + 60;
					}
					if (playerbar.GetHealth() != 100)
					{
						playerbar.SetHealth(playerbar.GetHealth() + 10);
					}
				}
			}
		}

	}
	for (CModel* pGoblin : enemies3)
	{
		if (player.HitTest(pGoblin))
		{
			if (iframes == 0)
			{
				if (parry == 0)
				{
					playerbar.SetHealth(playerbar.GetHealth() - 10);
					player.PlayAnimation("pain", 10, false);
					hurt.Play("hurt.wav");
					iframes = 15;

				}
				if (parry == 1)
				{
					parrysound.Play("parry.wav");
					player.PlayAnimation("wave", 10, true);
					goblindeath.Play("Goblin_death.wav");
					pGoblin->Delete();
					Waveprogression.SetHealth(Waveprogression.GetHealth() + 10);
					parrycooldown = 120;
					parry = false;
					if (reserve < 120)
					{
						reserve = reserve + 60;
					}
					if (playerbar.GetHealth() != 100)
					{
						playerbar.SetHealth(playerbar.GetHealth() + 10);
					}
				}
			}
		}

	}
	
	
	enemies.delete_if(deleted);
	enemies2.delete_if(deleted);
	enemies3.delete_if(deleted);
}

void CMyGame::EnemyControl()
{
	
	
	
		if (rand() % 100 == 0 && bossspawned == false)
		{
			// select randomly a side
			int s = rand() % 4;
			// clone the current enemy as a template
			CModel* pEnemy = enemy.Clone();
			if (s == 0) pEnemy->SetPosition(3000, float(1000 - rand() % 2000));
			else if (s == 1) pEnemy->SetPosition(-3000, float(1000 - rand() % 2000));
			else if (s == 2) pEnemy->SetPosition(float(1000 - rand() % 2000), 3000);
			else pEnemy->SetPosition(float(1000 - rand() % 2000), -3000);

			pEnemy->SetToFloor(0);
			pEnemy->PlayAnimation("run", 5, true); // select running animation
			// direct the enemy in the direction of the house
			pEnemy->SetDirectionAndRotationToPoint(house.GetX(), house.GetZ());
			pEnemy->SetSpeed(100);

			enemies.push_back(pEnemy);
		}
	
		if (rand() % 100 == 0 && bossspawned == false && wave >= 2)
		{
			// select randomly a side
			int s = rand() % 4;
			// clone the current enemy as a template
			CModel* pAlien = enemy2.Clone();
			if (s == 0) pAlien->SetPosition(3000, float(1000 - rand() % 2000));
			else if (s == 1) pAlien->SetPosition(-3000, float(1000 - rand() % 2000));
			else if (s == 2) pAlien->SetPosition(float(1000 - rand() % 2000), 3000);
			else pAlien->SetPosition(float(1000 - rand() % 2000), -3000);

			pAlien->SetToFloor(0);
			pAlien->PlayAnimation("run", 5, true); // select running animation
			
			pAlien->SetDirectionAndRotationToPoint(player.GetX(),player.GetY());
			pAlien->SetSpeed(100);

			enemies2.push_back(pAlien);
		}
		for (CModel* pAlien : enemies2)
		{
			
			pAlien->SetDirectionAndRotationToPoint(player.GetX(), player.GetZ());
		}
		// spawn the third enemy
		if (rand() % 100 == 0 && bossspawned == false && wave >= 3)
		{
			int s = rand() % 4;
			CModel* pGoblin = enemy3.Clone();
			if (s == 0) pGoblin->SetPosition(3000, float(1000 - rand() % 2000));
			else if (s == 1) pGoblin->SetPosition(-3000, float(1000 - rand() % 2000));
			else if (s == 2) pGoblin->SetPosition(float(1000 - rand() % 2000), 3000);
			else pGoblin->SetPosition(float(1000 - rand() % 2000), -3000);
			pGoblin->SetToFloor(0);
			pGoblin->PlayAnimation("run", 5, true);
			pGoblin->SetDirectionAndRotationToPoint(player.GetX(), player.GetZ());
			pGoblin->SetSpeed(200);
			enemies3.push_back(pGoblin);

		}
		// move the third enemy towards the player
		for (CModel* pGoblin : enemies3)
		{
			pGoblin->SetDirectionAndRotationToPoint(player.GetX(), player.GetZ());
		}
    // Enemy A.I.: Moving towards the house
	for (CModel* pEnemy : enemies)
	{
	 
	  pEnemy->SetDirectionAndRotationToPoint( house.GetX(), house.GetZ());
	  
	  // reduce health bar once an enemy has reached the house
	  if (house.HitTest(pEnemy)) 
	  {
		housebar.SetHealth(housebar.GetHealth()-10);
		pEnemy->Delete();
	  }
	}
	enemies.delete_if(deleted);
	// alien shots
	for (CModel* pAlien : enemies2)
	{
		if (rand() % 100 == 0)
		{
			shot.Play("shot.wav");
			CModel* EnemyShot = new CLine(pAlien->GetPositionV(), 50, CColor::Red());
			EnemyShot->SetDirectionAndRotationToPoint(player.GetX(), player.GetZ());
			EnemyShot->SetSpeed(2000);
			EnemyShot->Die(2000);
			enemyshots.push_back(EnemyShot);
		}
	}
	
}


void CMyGame::ShotsControl()
{
	
	// collision detection between shots and enemies
	for (CModel* pEnemy : enemies)
	{
	 for (CModel* pShot : shots)
	 {
		if (pEnemy->HitTest(pShot->GetPositionV()))
		{
			Waveprogression.SetHealth(Waveprogression.GetHealth() + 10);
			ogrodeath.Play("ogro_death.wav");
			score++;
			pEnemy->Delete();
			pShot->Delete();
			int g = rand() % 5+1;
			if (g == 1)
			{
				CModel* Pbox = box.Clone();
				Pbox->SetPositionV(pEnemy->GetPositionV());
				
				boxes.push_back(Pbox);
				
			}
			
			break;
		} 
		
	 }
    }
	// collision detection between shots and enemies2
	for (CModel* pAlien : enemies2)
	{
		for (CModel* pShot : shots)
		{
			if (pAlien->HitTest(pShot->GetPositionV()))
			{
				Waveprogression.SetHealth(Waveprogression.GetHealth() + 10);
				aliendeath.Play("alien_death.wav");
				score++;
				pAlien->Delete();
				pShot->Delete();
				int g = rand() % 5 + 1;
				if (g == 1)
				{
					CModel* Pbox = box.Clone();
					Pbox->SetPositionV(pAlien->GetPositionV());

					boxes.push_back(Pbox);

				}

				break;
			}

		}
	}
	// collision detection between shots and enemies3
	for (CModel* pGoblin : enemies3)
	{
		for (CModel* pShot : shots)
		{
			if (pGoblin->HitTest(pShot->GetPositionV()))
			{
				Waveprogression.SetHealth(Waveprogression.GetHealth() + 10);
				goblindeath.Play("Goblin_death.wav");
				score++;
				pGoblin->Delete();
				pShot->Delete();
				int g = rand() % 5 + 1;
				if (g == 1)
				{
					CModel* Pbox = box.Clone();
					Pbox->SetPositionV(pGoblin->GetPositionV());

					boxes.push_back(Pbox);

				}

				break;
			}

		}
	}
	// collision between enemy shots and player if the player isn't parrying they lose 10 health but if they are they fire a shot back using their own shots list
	for (CModel* EnemyShot : enemyshots)
	{
		if (player.HitTest(EnemyShot->GetPositionV()))
		{
			if (iframes == 0)
			{
				if (parry == 0)
				{
					playerbar.SetHealth(playerbar.GetHealth() - 10);
					hurt.Play("hurt.wav");
					player.PlayAnimation("pain", 10, true);
					iframes = 15;
					EnemyShot->Delete();
				}
				if (parry == 1)
				{
					CModel* pShot = new CLine(player.GetPositionV(), 50, CColor::Green());
					parrysound.Play("parry.wav");
					pShot->SetDirection(player.GetRotation());
					pShot->SetRotation(player.GetRotation());
					
					playerbar.SetHealth(playerbar.GetHealth() + 10);

					pShot->SetSpeed(2000);
					pShot->Die(2000);
					shots.push_back(pShot);
					parrycooldown = 120;
					parry = false;
					EnemyShot->Delete();
				}
			}
		}
	}
	

	

	
	// if the boss is spawned delete the enemies
	if (bossspawned == true)
	{
		enemies.delete_all();
		enemies2.delete_all();
		enemies3.delete_all();
	}
	
    enemies.delete_if(deleted);
	enemies2.delete_if(deleted);
	enemies3.delete_if(deleted);
    shots.delete_if(deleted);
	enemyshots.delete_if(deleted);
    
    for (CModel* pShot: shots)
	{
		// remove shots when hitting the house	    
		if (pShot->HitTest(&house))
		{
			pShot->Delete();
		}
	 
	}
    shots.delete_if(deleted);
	enemyshots.delete_if(deleted);
}

void CMyGame::Wavecontrol()
{
	if (Waveprogression.GetHealth() == 100)
	{
		bossready = true;
		
	}
	if (bosskilled == true)
	{
		Waveprogression.SetHealth(0);
		bosskilled = false;
		bossspawned = false;
		bossready = false;
		playerbar.SetHealth(100);
		housebar.SetHealth(housebar.GetHealth() + 10);
		bosstheme.Stop();
		normaltheme.Play("normal_music.wav", -1);
		wave++;
	}
	if (wave >= 4 && endless == false)
	{
		isgamewon = true;
		GameOver();
	}
}

void CMyGame::boxcontrol()
{
	for (CModel* Pbox : boxes)
	{
		Pbox->MoveTo(player.GetX(), player.GetZ(), 100);
		Pbox->SetDirectionAndRotationToPoint(player.GetX(), player.GetZ());
		if (player.HitTest(Pbox) && reserve < 120)
		{
			reserve = reserve + 30;
			
			Pbox->Delete();

		}
		if ((player.HitTest(Pbox) && reserve > 120) || player.HitTest(Pbox) && player.GetHealth() > 100)
		{
			Pbox->Delete();
		}
		if (player.HitTest(Pbox) && playerbar.GetHealth() < 100)
		{
			playerbar.SetHealth(playerbar.GetHealth() + 10);
			Pbox->Delete();
		}
	}
	boxes.delete_if(deleted);
}

void CMyGame::bosscontrol()
{
	// spawning the first boss
	if (bossready == true && bossspawned == false && Waveprogression.GetHealth() == 100 && wave == 1)// Spawning the boss
	{
		bossspawned = true;
		
		
		


		normaltheme.Stop();
		bosstheme.Play("boss_music.wav",-1);
			bossbar.SetSize(100, 15);
			bossbar.SetHealth(100);
			int s = rand() % 4;
			CModel* pBoss = boss.Clone();
			if (s == 0) pBoss->SetPosition(3000, float(1000 - rand() % 2000));
			else if (s == 1) pBoss->SetPosition(-3000, float(1000 - rand() % 2000));
			else if (s == 2) pBoss->SetPosition(float(1000 - rand() % 2000), 3000);
			else pBoss->SetPosition(float(1000 - rand() % 2000), -3000);

			pBoss->SetToFloor(0);
			pBoss->PlayAnimation(40,45, 5, true); // select running animation
			// direct the enemy in the direction of the house
			pBoss->SetDirectionAndRotationToPoint(house.GetX(), house.GetZ());
			pBoss->SetSpeed(100);
			
			bosses.push_back(pBoss);
			bosskilled = false;
	}
	// spawning the second boss
	if (bossready == true && bossspawned == false && Waveprogression.GetHealth() == 100 && wave == 2)// Spawning the boss
	{
		normaltheme.Stop();
		bosstheme.Play("boss_music.wav", -1);
		bossspawned = true;
		bossbar2.SetSize(100, 15);
		bossbar2.SetHealth(100);
		int s = rand() % 4;
		CModel* pBoss2 = boss2.Clone();
		if (s == 0) pBoss2->SetPosition(3000, float(1000 - rand() % 2000));
		else if (s == 1) pBoss2->SetPosition(-3000, float(1000 - rand() % 2000));
		else if (s == 2) pBoss2->SetPosition(float(1000 - rand() % 2000), 3000);
		else pBoss2->SetPosition(float(1000 - rand() % 2000), -3000);
		pBoss2->SetToFloor(0);
		pBoss2->PlayAnimation(40, 45, 5, true);
		pBoss2->SetDirectionAndRotationToPoint(player.GetX(), player.GetZ());
		pBoss2->SetSpeed(100);
		bosses2.push_back(pBoss2);
		bosskilled = false;

	}
	// spawning the third boss
	if (bossready == true && bossspawned == false && Waveprogression.GetHealth() == 100 && wave >= 3)// Spawning the boss
	{
		normaltheme.Stop();
		bosstheme.Play("boss_music.wav", -1);
		bossspawned = true;
		bossbar3.SetSize(100, 15);
		bossbar3.SetHealth(100);
		int s = rand() % 4;
		CModel* pBoss3 = boss3.Clone();
		if (s == 0) pBoss3->SetPosition(3000, float(1000 - rand() % 2000));
		else if (s == 1) pBoss3->SetPosition(-3000, float(1000 - rand() % 2000));
		else if (s == 2) pBoss3->SetPosition(float(1000 - rand() % 2000), 3000);
		else pBoss3->SetPosition(float(1000 - rand() % 2000), -3000);
		pBoss3->SetToFloor(0);
		pBoss3->PlayAnimation(40, 45, 5, true);
		pBoss3->SetDirectionAndRotationToPoint(player.GetX(), player.GetZ());
		pBoss3->SetSpeed(100);
		bosses3.push_back(pBoss3);
		bosskilled = false;

	}
	
	
	
	
	for (CModel* pBoss : bosses)
	{

		pBoss->SetDirectionAndRotationToPoint(house.GetX(), house.GetZ());
		
		if (house.HitTest(pBoss))
		{
			GameOver();
			pBoss->Delete();
		}
	}
	bosses.delete_if(deleted);
	// if boss hit tests player they lose 50 health
	for (CModel* pBoss : bosses)
	{
		if (player.HitTest(pBoss))
		{
			if (iframes == 0)
			{
				playerbar.SetHealth(playerbar.GetHealth() - 50);
				player.PlayAnimation("pain", 10, false);
				hurt.Play("hurt.wav");
				iframes = 15;
			}
		}
	}
	// boss2 collision detection with player
	for (CModel* pBoss2 : bosses2)
	{

		pBoss2->SetDirectionAndRotationToPoint(player.GetX(), player.GetZ());

		if (player.HitTest(pBoss2))
		{
			if (iframes == 0)
			{
				playerbar.SetHealth(playerbar.GetHealth() - 50);
				player.PlayAnimation("pain", 10, false);
				hurt.Play("hurt.wav");
				iframes = 15;
			}
		}
	}
	// boss3 collision detection with player
	for (CModel* pBoss3 : bosses3)
	{

		pBoss3->SetDirectionAndRotationToPoint(player.GetX(), player.GetZ());

		if (player.HitTest(pBoss3))
		{
			if (iframes == 0)
			{
				playerbar.SetHealth(playerbar.GetHealth() - 50);
				player.PlayAnimation("pain", 10, false);
				hurt.Play("hurt.wav");
				iframes = 15;
			}
		}
	}
	
	// collision detection between shots and bosses
	for (CModel* pBoss : bosses)
	{
		for (CModel* pShot : shots)
		{
			if (pBoss->HitTest(pShot->GetPositionV()))
			{
				bossbar.SetHealth(bossbar.GetHealth() - 10);
				bosshit.Play("hit.wav");
				pBoss->PlayAnimation("pain", 10, false);
				pBoss->PlayAnimation(40, 45, 5, true);
				pShot->Delete();
				

			}
			

		}
	}
	for (CModel* pBoss2 : bosses2)
	{
		for (CModel* pShot : shots)
		{
			if (pBoss2->HitTest(pShot->GetPositionV()))
			{
				bossbar2.SetHealth(bossbar2.GetHealth() - 10);
				pBoss2->PlayAnimation("Pain", 10, false);
				pBoss2->PlayAnimation(40, 45, 5, true);
				bosshit.Play("hit.wav");
				pShot->Delete();
			}
		}
	}
	for (CModel* pBoss3 : bosses3)
	{
		for (CModel* pShot : shots)
		{
			if (pBoss3->HitTest(pShot->GetPositionV()))
			{
				bossbar3.SetHealth(bossbar3.GetHealth() - 5);
				pBoss3->PlayAnimation("pain", 10, false);
				pBoss3->PlayAnimation(40, 45, 5, true);
				bosshit.Play("hit.wav");
				pShot->Delete();
			}
		}
	}
	// the second boss shoots at the player
	for (CModel* pBoss2 : bosses2)
	{
		if (rand() % 100 == 0)
		{
			CModel* Bossshot = new CLine(pBoss2->GetPositionV(), 50, CColor::Blue());
			shot.Play("shot.wav");
			Bossshot->SetDirectionAndRotationToPoint(player.GetX(), player.GetZ());
			Bossshot->SetSpeed(2000);
			Bossshot->Die(2000);
			bossbullets.push_back(Bossshot);
		}
	}
	// if the player is hit by the boss they lose 50 health
	for (CModel* Bossshot : bossbullets)
	{
		if (player.HitTest(Bossshot->GetPositionV()))
		{
			if (iframes == 0)
			{
				playerbar.SetHealth(playerbar.GetHealth() - 10);
				player.PlayAnimation("pain", 10, false);
				hurt.Play("hurt.wav");
				iframes = 15;
				Bossshot->Delete();
			}
		}
	}
	bossbullets.delete_if(deleted);
	shots.delete_if(deleted);
	
	bosses.delete_if(deleted);
	bosses2.delete_if(deleted);
	bosses3.delete_if(deleted);
	// code for putting the boss health bar next to the boss
	for (CModel* pBoss : bosses)
	{
		CVector bossind = WorldToScreenCoordinate(pBoss->GetPositionV());
		bossbar.SetPosition(bossind.x, bossind.y - 40);
	}
	for (CModel* pBoss : bosses2)
	{
		CVector bossind = WorldToScreenCoordinate(pBoss->GetPositionV());
		bossbar2.SetPosition(bossind.x, bossind.y - 40);
	}
	for (CModel* pBoss : bosses3)
	{
		CVector bossind = WorldToScreenCoordinate(pBoss->GetPositionV());
		bossbar3.SetPosition(bossind.x, bossind.y - 40);
	}

	for (CModel* pBoss : bosses)
	{
		if (bossbar.GetHealth() == 0)
		{
			
			bossdeath.Play("Alien_boss_death.wav");
			
			pBoss->Delete();
			bosskilled = true;
			
		}
		
	}
	for (CModel* pBoss : bosses2)
	{
		if (bossbar2.GetHealth() == 0)
		{
			bossdeath2.Play("droid_death.wav");
			
			pBoss->Delete();
			bosskilled = true;

		}
	}
	for (CModel* pBoss : bosses3)
	{
		if (bossbar3.GetHealth() == 0)
		{
			bossdeath3.Play("ogre_boss_death.wav");
			
			pBoss->Delete();
			bosskilled = true;
		}
	}
	
	
	
	
	
		
	
}










//-----------------  Draw 2D overlay ----------------------
void CMyGame::OnDraw(CGraphics* g)
{
	if (IsMenuMode())
	{
	ShowMouse();
	if (iscontrols == false && ismodeselect == false)
	{
		startScreen.Draw(g);
		startButton.Draw(g);
		startButton.SetPosition(900, 650);
		controlsButton.Draw(g);
		controlsButton.SetPosition(900, 540);
		
		quitButton.Draw(g);
		quitButton.SetPosition(900, 430);
	}
	 // if the control menu is true draw it 
	 if (iscontrols == true)
	 {
		 controlsScreen.Draw(g);
		 menuButton.Draw(g);
		 menuButton.SetPosition(900, 100);
	 }
	 // if the mode select menu is true draw it
	 if (ismodeselect == true)
	 {
		 Modeselectscreen.Draw(g);
		 normalButton.Draw(g);
		 normalButton.SetPosition(700, 650);
		 endlessButton.Draw(g);
		 endlessButton.SetPosition(1150, 650);
		 menuButton.Draw(g);
		 menuButton.SetPosition(900, 450);
	 }
	 return;
	}
	if (!IsMenuMode())
	{
		HideMouse();
	}
	if (!IsGameOver() && !IsMenuMode())
	{


		// draw score
		font.SetSize(32); font.SetColor(CColor::Red()); font.DrawText(10, Height - 30, "Score:"); font.DrawNumber(120, Height - 30, score);
		// draw ammo and mag counter
		if (ammo >= 20 && ammo <= 30)
		{
			font.SetSize(32); font.SetColor(CColor::Green()); font.DrawText(0, Height - 1000, "Ammo:"); font.DrawNumber(80, Height - 1000, ammo); font.DrawText(120, Height - 1000, "/"); font.DrawNumber(140, Height - 1000, reserve);
		}
		if (ammo >= 11 && ammo <= 19)
		{
			font.SetSize(32); font.SetColor(CColor::Yellow()); font.DrawText(0, Height - 1000, "Ammo:"); font.DrawNumber(80, Height - 1000, ammo); font.DrawText(120, Height - 1000, "/"); font.DrawNumber(140, Height - 1000, reserve);
		}
		if (ammo >= 0 && ammo <= 10)
		{
			font.SetSize(32); font.SetColor(CColor::Red()); font.DrawText(0, Height - 1000, "Ammo:"); font.DrawNumber(80, Height - 1000, ammo); font.DrawText(120, Height - 1000, "/"); font.DrawNumber(140, Height - 1000, reserve);
		}
		// if the gun is empty 
		if (ammo == 0 && reserve != 0)
		{
			font.SetSize(32); font.SetColor(CColor::Red()); font.DrawText(0, Height - 930, "Press R to reload");
		}
		if (ammo == 0 && reserve == 0)
		{
			font.SetSize(32); font.SetColor(CColor::Red()); font.DrawText(0, Height - 930, "Out of ammo");
		}
		// parry condtion 
		if (parry == 1)
		{
			CVector meleeind = WorldToScreenCoordinate(player.GetPositionV());
			font.SetSize(20); font.SetColor(CColor::Red()); font.DrawText(meleeind.x - 40, meleeind.y - 65, "Parrying");
		}

		if (parrycooldown == 0)
		{
			font.SetSize(32); font.SetColor(CColor::Green()); font.DrawText(0, Height - 970, "Parry ready");
		}
		if (parrycooldown != 0)
		{
			font.SetSize(32); font.SetColor(CColor::Red()); font.DrawText(0, Height - 970, "Parry is on cool down for: "); font.DrawNumber(430, Height - 970, parrycooldown / 30);  font.DrawText(450, Height - 970, "seconds");
		}
		if (reload != 0)
		{
			font.SetSize(32); font.SetColor(CColor::Red()); font.DrawText(0, Height - 950, "Reloading...");
		}

		// draw GAME OVER if game over

		// debug
		CVector pos = WorldToScreenCoordinate(player.GetPositionV());
		playerbar.SetPosition(pos.x, pos.y - 40);

		CVector pos2 = WorldToScreenCoordinate(house.GetPositionV());

		housebar.SetPosition(pos2.x, pos2.y + 40);
		font.SetSize(32); font.SetColor(CColor::Red()); font.DrawText(840, 1000, "Wave: "); font.DrawNumber(920, 1000, wave);
		Waveprogression.SetPosition(900, 1050);
		playerbar.Draw(g);
		housebar.Draw(g);
		Waveprogression.Draw(g);
		if (bossready == true && wave == 1 && !bosskilled)
		{
			bossbar.Draw(g);

		}
		if (bossready == true && wave == 2 && !bosskilled)
		{
			bossbar2.Draw(g);
		}
		if (bossready == true && wave >= 3 && !bosskilled)
		{
			bossbar3.Draw(g);
		}
		if (isaiming == true)
		{
			crosshair.Draw(g);
		}
		
	}
	if (!IsMenuMode() && IsPaused())
	{
		ShowMouse();
		font.SetSize(64); font.SetColor(CColor::Red()); font.DrawText(850, 750, "PAUSED");
		menuButton.Draw(g);
		menuButton.SetPosition(950, 450);
		quitButton.Draw(g);
		quitButton.SetPosition(950, 300);
		resumeButton.Draw(g);
		resumeButton.SetPosition(950, 600);
	}

	if (IsGameOver() && isgamewon == false)
	{
		ShowMouse();
		font.SetSize(64); font.SetColor(CColor::Red()); font.DrawText(750, 750, "GAME OVER");
		menuButton.Draw(g);
		menuButton.SetPosition(650, 300);
		quitButton.Draw(g);
		quitButton.SetPosition(1150, 300);
	}
	if (isgamewon == true && IsGameOver())
	{
		endScreen.Draw(g);
		ShowMouse();
		// draw menu button and the quit button
		menuButton.Draw(g);
		menuButton.SetPosition(650, 300);
		quitButton.Draw(g);
		quitButton.SetPosition(1150, 300);
	}
}

// ----------------   Draw 3D world -------------------------
void CMyGame::OnRender3D(CGraphics* g)
{
	CameraControl(g);
    
	// ------- Draw your 3D Models here ----------
	
	floor.Draw(g);

	outerWalls.Draw(g);
	
	enemies.Draw(g);

	player.Draw(g);
	
	shots.Draw(g);
	enemyshots.Draw(g);
	house.Draw(g);
	bossbullets.Draw(g);
	boxes.Draw(g);
	bosses.Draw(g);
	bosses2.Draw(g);
	bosses3.Draw(g);
	enemies2.Draw(g);
	enemies3.Draw(g);

	//ShowBoundingBoxes();
	
	
}

void CMyGame::CameraControl(CGraphics* g)
{
	world.rotation.x = 60;     // tilt: rotation of game world around x-axis
	world.rotation.y = -35;    // rotation: rotation of game world around y-axis
	world.scale = 1.0f;	       // scaling the game world

	// ------ Global Transformation Functions -----
	glScalef(world.scale, world.scale, world.scale);  // scale the game world
	glTranslatef(world.position.x, world.position.y, world.position.z);  // translate game world
	glRotatef(world.rotation.x, 1, 0, 0);	// rotate game world around x-axis
	glRotatef(world.rotation.y, 0, 1, 0);	// rotate game world around y-axis

	// -----  Player camera control ----------
	glTranslatef(-player.GetX(), 0, -player.GetZ());    // translate game world to player position  
	
	UpdateView();
	Light.Apply();
}




// called at the start of a new game - display menu here
void CMyGame::OnDisplayMenu()
{
		bosstheme.Stop();
		normaltheme.Stop();
		maintheme.Stop();
		maintheme.Play("Menu_music.wav", -1);
}

// called when Game Mode entered
void CMyGame::OnStartGame()
{
     OnStartLevel(1);	
   // clear all the lists apart from the outer walls
	 enemies.delete_all();
	 enemies2.delete_all();
	 enemies3.delete_all();
	 shots.delete_all();
	 enemyshots.delete_all();
	 boxes.delete_all();
	 bosses.delete_all();
	 bosses2.delete_all();
	 bosses3.delete_all();
	 bossbullets.delete_all();
	 Waveprogression.SetHealth(0);
	 bossready = false;
	 bossspawned = false;
	 bosskilled = false;
	 wave = 1;
	 score = 0;
	 iframes = 0;
	 parrycooldown = 0;
	 firerate = 0;
	 reload = 0;
	 bulletsfired = 0;
	 parry = false;
	 housebar.SetHealth(100);
	 playerbar.SetHealth(100);
	 player.SetPosition(400, 100, 300);
	 ammo = 30;
	 reserve = 120;
	 

}


// called when Game is Over
void CMyGame::OnGameOver()
{
	normaltheme.Stop();
	bosstheme.Stop();
	maintheme.Play("Menu_music.wav", -1);
	
}

// one time termination code
void CMyGame::OnTerminate()
{
	
}

// -------    Keyboard Event Handling ------------

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (!IsMenuMode())
	{
		if (sym == SDLK_ESCAPE)
		{
			if (IsPaused())
			{
				pause.Play("pause.wav");
				ResumeGame();
			}
			else
			{
				unpause.Play("resume.wav");
				PauseGame();
			}
		}
	}
	
	if (sym == SDLK_F2) NewGame();
	if (sym == SDLK_r)
	{
		
	 if  (ammo != 30 && reserve != 0 && reload == 0)
	{
		 reloadsound.Play("Reload.wav");
		 ammo = 30;
		 reserve = reserve - bulletsfired;
		 bulletsfired = 0;
		 reload = 45;
		
	}
	 
	}
	if (sym == SDLK_f && parrycooldown == 0 && IsKeyUp(SDLK_w))
	{
		parry = true;
		player.PlayAnimation("salute", 10, false);
	}
	if (sym == SDLK_w)
	{
		parry = false;
	}
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_f)
	{
		parry = false;
	}
}

// -----  Mouse Events Handlers -------------

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
	
	if (!IsGameOver())
	{
		crosshair.SetPosition(x, y);
	  // project screen to floor coordinates
	  CVector pos=ScreenToFloorCoordinate(x,y);
	 
	  player.SetDirectionAndRotationToPoint(pos.x,pos.z);
	  
    }
	
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{    
	if (IsMenuMode())
	{
		if (iscontrols == false && ismodeselect == false)
		{
			if (startButton.HitTest(x, y))
			{
				select.Play("select.wav");
				ismodeselect = true;
			}
			if (controlsButton.HitTest(x, y))
			{
				select.Play("select.wav");
				iscontrols = true;
				
			}
			if (quitButton.HitTest(x, y))
			{
				StopGame();
			}
			
			
			
		}
		if (iscontrols == true)
		{
			if (menuButton.HitTest(x, y))
			{
				exit.Play("exit.wav");
				iscontrols = false;
			}
		}
		if (ismodeselect == true)
		{
			if (normalButton.HitTest(x, y))
			{
				select.Play("select.wav");
				ismodeselect = false;
				endless = false;
				maintheme.Stop();
				normaltheme.Play("Normal_music.wav", -1);
				StartGame();
			}
			if (endlessButton.HitTest(x, y))
			{
				select.Play("select.wav");
				ismodeselect = false;
				endless = true;
				maintheme.Stop();
				normaltheme.Play("Normal_music.wav", -1);
				StartGame();
			}
			if (menuButton.HitTest(x, y))
			{
				exit.Play("exit.wav");
				ismodeselect = false;
			}
		}
	}
	if (IsGameOver() && !IsMenuMode())
	{
		
		if (menuButton.HitTest(x, y))
		{
			select.Play("select.wav");
			NewGame();
		}
		
		if (quitButton.HitTest(x, y))
		{
			StopGame();
		}

	}
	if (!IsMenuMode() && IsPaused())
	{
		if (quitButton.HitTest(x, y))
		{
			StopGame();
		}
		if (menuButton.HitTest(x, y))
		{
			NewGame();
			
			exit.Play("exit.wav");
		}
		if (resumeButton.HitTest(x, y))
		{
			ResumeGame();
			exit.Play("exit.wav");
		}
	}
	else if (ammo > 0 && firerate == 0 && reload == 0 && isaiming == true)
	{
		firerate = 10;
		shot.Play("shot.wav");
		player.PlayAnimation("attack", 10, true);
	  // create a new shot as a short line
	  CModel* pShot = new CLine(player.GetPositionV(), 50);
	  // rotation and direction same as the player
	  pShot->SetDirection(player.GetRotation());
	  pShot->SetRotation(player.GetRotation());
	  pShot->SetSpeed(2000);
	  pShot->Die(2000);
	  shots.push_back(pShot);
	  ammo--;
	  bulletsfired++;
    }
		
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
	isaiming = true;
	
	
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
	isaiming = false;
	
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
