#include "BattleTank.h"

Keyboard::Keyboard()
{
	wIsPressed = false;
	sIsPressed = false;
	aIsPressed = false;
	dIsPressed = false;
	jIsPressed = false;
	kIsPressed = false;

	upIsPressed = false;
	downIsPressed = false;
	leftIsPressed = false;
	rightIsPressed = false;
	numpad0IsPressed = false;
	rctrlIsPressed = false;
	rshiftIsPressed = false;

	enterIsPressed = false;
	spaceIsPressed = false;
	escIsPressed = false;

	wIsReleased = false;
	sIsReleased = false;
	aIsReleased = false;
	kIsReleased = false;
	dIsReleased = false;
	jIsReleased = false;

	upIsReleased = false;
	downIsReleased = false;
	leftIsReleased = false;
	rightIsReleased = false;
	numpad0IsReleased = false;
	rctrlIsReleased = false;
	rshiftIsReleased = false;

	spaceIsReleased = false;
	enterIsReleased = false;
	escIsReleased = false;
}

// 按下按键---------------------------------------------------
// Player_1 Keyboard input
void Keyboard::OnWPressed()
{
	wIsPressed = true;
}

void Keyboard::OnSPressed()	
{
	sIsPressed = true;
}

void Keyboard::OnAPressed()	
{
	aIsPressed = true;
}

void Keyboard::OnDPressed()	
{
	dIsPressed = true;
}

void Keyboard::OnJPressed()	
{
	jIsPressed = true;
}

void Keyboard::OnKPressed()	
{
	kIsPressed = true;
}

// Player_2 Keyboard input
void Keyboard::OnUpPressed()
{
	upIsPressed = true;
}
void Keyboard::OnDownPressed()
{
	downIsPressed = true;
}
void Keyboard::OnLeftPressed()
{
	leftIsPressed = true;
}
void Keyboard::OnRightPressed()
{
	rightIsPressed = true;
}

void Keyboard::OnNumpad0Pressed()
{
	numpad0IsPressed = true;
}

void Keyboard::OnRightCtrlPressed()
{
	rctrlIsPressed = true;
}

void Keyboard::OnRightShiftPressed()
{
	rshiftIsPressed = true;
}

// Global input
void Keyboard::OnSpacePressed()
{
	spaceIsPressed = true;
}
void Keyboard::OnEnterPressed()
{
	enterIsPressed = true;
}
void Keyboard::OnEscPressed()
{
	escIsPressed = true;
}

// 按键释放---------------------------------------------------
// Player_1 Keyboard input
void Keyboard::OnWReleased()
{
	wIsPressed = false;
	wIsReleased = true;
}

void Keyboard::OnSReleased()
{
	sIsPressed = false;
	sIsReleased = true;
}

void Keyboard::OnAReleased()
{
	aIsPressed = false;
	aIsReleased = true;
}

void Keyboard::OnDReleased()
{
	dIsPressed = false;
	dIsReleased = true;
}

void Keyboard::OnJReleased()
{
	jIsPressed = false;
	jIsReleased = true;
}

void Keyboard::OnKReleased()
{
	kIsPressed = false;
	kIsReleased = true;
}

// Player_2 Keyboard input
void Keyboard::OnUpReleased()
{
	upIsPressed = false;
}
void Keyboard::OnDownReleased()
{
	downIsPressed = false;
}
void Keyboard::OnLeftReleased()
{
	leftIsPressed = false;
}
void Keyboard::OnRightReleased()
{
	rightIsPressed = false;
}
void Keyboard::OnNumpad0Released()
{
	numpad0IsPressed = false;
}

void Keyboard::OnRightCtrlReleased()
{
	rctrlIsPressed = false;
}

void Keyboard::OnRightShiftReleased()
{
	rshiftIsPressed = false;
}

// Global input
void Keyboard::OnSpaceReleased()
{
	spaceIsPressed = false;
	spaceIsReleased = true;
}
void Keyboard::OnEnterReleased()
{
	enterIsPressed = false;
	enterIsReleased = true;
}
void Keyboard::OnEscReleased()
{
	escIsPressed = false;
	escIsReleased = true;
}
	

 // 返回按键状态------------------------------------------------------

bool Keyboard::IsWPressed() const
{
	return this->wIsPressed;
}

bool Keyboard::IsSPressed() const
{
	return this->sIsPressed;
}
bool Keyboard::IsAPressed() const
{	 
	return this->aIsPressed;
}	 
	 
bool Keyboard::IsDPressed() const
{	
	return this->dIsPressed;
}	
	
bool Keyboard::IsJPressed() const
{	 
	return this->jIsPressed;
}	 
	 
bool Keyboard::IsKPressed() const
{
	return this->kIsPressed;
}

bool Keyboard::IsUpPressed() const
{
	return this->upIsPressed;
}
bool Keyboard::IsDownPressed() const
{
	return this->downIsPressed;
}
bool Keyboard::IsLeftPressed() const
{
	return this->leftIsPressed;
}
bool Keyboard::IsRightPressed() const
{
	return this->rightIsPressed;
}

bool Keyboard::IsNumpad0Pressed() const 
{
	return this->numpad0IsPressed;
}

bool Keyboard::IsRightCtrlPressed() const
{
	return this->rctrlIsPressed;
}

bool Keyboard::IsRightShiftPressed() const
{
	return this->rshiftIsPressed;
}

bool Keyboard::IsSpacePressed() const
{
	return this->spaceIsPressed;
}
bool Keyboard::IsEnterPressed() const
{
	return this->enterIsPressed;
}
bool Keyboard::IsEscPressed() const
{
	return this->escIsPressed;
}

Tank::Tank(): bullet(1), m_pTexture(4, NULL)
{
	direction = 3;
	isEnemy = 1;
	position = D2D1::RectF(0.0F, 0.0F, 60.0F, 60.0F);
	MoveUp = 0;
	MoveDown = 0;
	MoveLeft = 0;
	MoveRight = 0;

	collide_with_other_tank = 0;
	alive_frame = 0;
	stuck_frame = 0;
	health = 1;
	appear_time = 0;
}

Tank::Tank(bool isEnemy, D2D1_RECT_F pos, int _health) : bullet(1), m_pTexture(4, NULL)
{
	direction = 0;
	isEnemy = 0;
	position = pos;
	MoveUp = 0;
	MoveDown = 0;
	MoveLeft = 0;
	MoveRight = 0;

	collide_with_other_tank = 0;
	alive_frame = 0;
	stuck_frame = 0;
	health = _health;
	appear_time = 0;
}



/*
void Tank::MoveReset()
{
	MoveUp = 0;
	MoveDown = 0;
	MoveLeft = 0;
	MoveRight = 0;
}

void Tank::MoveTankUp()
{
	this->direction = 0;
	if (this->position.top >= 30 + 3)
	{
		this->position.bottom -= MOVEMENT_SPEED;
		this->position.top -= MOVEMENT_SPEED;
	}
}

void Tank::MoveTankDown()
{
	this->direction = 3;
	if (this->position.bottom <= 810 - 1)
	{
		this->position.bottom += MOVEMENT_SPEED;
		this->position.top += MOVEMENT_SPEED;
	}
}

void Tank::MoveTankLeft()
{
	this->direction = 1;
	if (this->position.left >= 30 + 3)
	{
		this->position.left -= MOVEMENT_SPEED;
		this->position.right -= MOVEMENT_SPEED;
	}
}

void Tank::MoveTankRight()
{
	this->direction = 2;
	if (this->position.right <= 810 - 1)
	{
		this->position.left += MOVEMENT_SPEED;
		this->position.right += MOVEMENT_SPEED;
	}
}
*/

void Tank::MoveTankUpPixel(float pixel)
{
	this->direction = 0;
	if (this->position.top >= 30)
	{
		this->position.bottom -= pixel;
		this->position.top -= pixel;
	}
}

void Tank::MoveTankDownPixel(float pixel)
{
	this->direction = 3;
	if (this->position.bottom <= 810)
	{
		this->position.bottom += pixel;
		this->position.top += pixel;
	}
}

void Tank::MoveTankLeftPixel(float pixel)
{
	this->direction = 1;
	if (this->position.left >= 30)
	{
		this->position.left -= pixel;
		this->position.right -= pixel;
	}
}

void Tank::MoveTankRightPixel(float pixel)
{
	this->direction = 2;
	if (this->position.right <= 810 )
	{
		this->position.left += pixel;
		this->position.right += pixel;
	}
}

int Tank::GetDirection() const
{
	return this->direction;
}

D2D1_RECT_F Tank::GetPosition() const
{
	return this->position;
}

Bullet::Bullet()
{
	direction = 0;
	level = 1;
	bullet_speed = 7;
}

Bullet::Bullet(Tank tank, int bullet_level, int bullet_speed)
{
	this->level = bullet_level;
	this->bullet_speed = bullet_speed;
	direction = tank.GetDirection();
	position = tank.GetPosition();
	if (direction == 1)	//left
	{
		position.left -= 10;
		position.right = position.left + 10;
		position.top += 25;
		position.bottom = position.top + 10;
	}
	else if (direction == 2)	//right
	{
		position.left += 60;
		position.right = position.left + 10;
		position.top += 25;
		position.bottom = position.top + 10;
	}
	else if (direction == 3)	//down
	{
		position.left += 25;
		position.right = position.left + 10;
		position.top += 60;
		position.bottom = position.top + 10;
	}
	else
	{
		position.left += 25;
		position.right = position.left + 10;
		position.top -= 10;
		position.bottom = position.top - 10;
	}
}

bool Bullet::Move()
{
	bool flag = true;

	if (direction == 1)	//left
	{
		if (int(position.top + 5.0F) % 30 == 0)
		{	
			int i = int(position.top + 5.0F) / 30;
			int j = int(position.left) / 30;
			switch (current_map[i][j])
			{
			case 1:
				current_map[i][j] = 0;
				flag = false;
				break;
			case 2:
				flag = false;
				break;
			case 9:
				gameover = true;
				flag = false;
				break;
			}

			switch (current_map[i - 1][j])
			{
			case 1:
				current_map[i - 1][j] = 0;
				flag = false;
				break;
			case 2:
				flag = false;
				break;
			case 9:
				gameover = true;
				flag = false;
				break;
			}
		}
		else
		{
			int i = int(position.top + 5.0F) / 30;
			int j = int(position.left) / 30;
			switch (current_map[i][j])
			{
			case 1:
				current_map[i][j] = 0;
				flag = false;
				break;
			case 2:
				flag = false;
				break;
			case 9:
				gameover = true;
				flag = false;
				break;
			}
		}

		if (position.left <= 30 + bullet_speed || flag == false)
			return false;
		position.left -= bullet_speed;
		position.right -= bullet_speed;		
	}

	else if (direction == 2)	//right
	{
		if (int(position.top + 5.0F) % 30 == 0)
		{
			int i = int(position.top + 5.0F) / 30;
			int j = int(position.right) / 30;
			switch (current_map[i][j])
			{
			case 1:
				current_map[i][j] = 0;
				flag = false;
				break;
			case 2:
				flag = false;
				break;
			case 9:
				gameover = true;
				flag = false;
				break;
			}

			switch (current_map[i - 1][j])
			{
			case 1:
				current_map[i - 1][j] = 0;
				flag = false;
				break;
			case 2:
				flag = false;
				break;
			case 9:
				gameover = true;
				flag = false;
				break;
			}
		}
		else
		{
			int i = int(position.top + 5.0F) / 30;
			int j = int(position.right) / 30;
			switch (current_map[i][j])
			{
			case 1:
				current_map[i][j] = 0;
				flag = false;
				break;
			case 2:
				flag = false;
				break;
			case 9:
				gameover = true;
				flag = false;
				break;
			}
		}

		if (position.right >= 810 - bullet_speed || flag == false)
			return false;
		position.left += bullet_speed;
		position.right += bullet_speed;
	}

	else if (direction == 3)	//down
	{
		if (int(position.left + 5.0F) % 30 == 0)
		{
			int i = int(position.bottom) / 30;
			int j = int(position.left + 5.0F) / 30;
			switch (current_map[i][j])
			{
			case 1:
				current_map[i][j] = 0;
				flag = false;
				break;
			case 2:
				flag = false;
				break;
			case 9:
				gameover = true;
				flag = false;
				break;
			}

			switch (current_map[i][j - 1])
			{
			case 1:
				current_map[i][j - 1] = 0;
				flag = false;
				break;
			case 2:
				flag = false;
				break;
			case 9:
				gameover = true;
				flag = false;
				break;
				flag = false;
				break;
			}
		}
		else
		{
			int i = int(position.bottom) / 30;
			int j = int(position.left + 5.0F) / 30;
			switch (current_map[i][j])
			{
			case 1:
				current_map[i][j] = 0;
				flag = false;
				break;
			case 2:
				flag = false;
				break;
			case 9:
				gameover = true;
				flag = false;
				break;
			}
		}

		if (position.bottom >= 810 - bullet_speed || flag == false)
			return false;
		position.top += bullet_speed;
		position.bottom = position.bottom + bullet_speed;		
	}

	else
	{
		if (int(position.left + 5.0F) % 30 == 0)
		{	// current_map里的值: 0为可通过陆地，1为红砖，2钢墙，3为水，4为树荫，5为敌人，9为Base
			int i = int(position.top) / 30;
			int j = int(position.left + 5.0F) / 30;
			switch (current_map[i][j])
			{
			case 1:
				current_map[i][j] = 0;
				flag = false;
				break;
			case 2:
				flag = false;
				break;
			case 9:
				gameover = true;
				flag = false;
				break;
			}

			switch (current_map[i][j - 1])
			{
			case 1:
				current_map[i][j - 1] = 0;
				flag = false;
				break;
			case 2:
				flag = false;
				break;
			case 9:
				gameover = true;
				flag = false;
				break;
			}
		}
		else
		{
			int i = int(position.top) / 30;
			int j = int(position.left + 5.0F) / 30;
			switch (current_map[i][j])
			{
			case 1:
				current_map[i][j] = 0;
				flag = false;
				break;
			case 2:
				flag = false;
				break;
			case 9:
				gameover = true;
				flag = false;
				break;
			}
		}

		if ( (position.top <= 30 + bullet_speed) || (flag == false) )
			return false;
		position.top -= bullet_speed;
		position.bottom = position.bottom - bullet_speed;
	}

	return true;
}