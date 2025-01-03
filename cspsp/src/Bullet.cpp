#include "Bullet.h"
#include "Person.h"

JRenderer* Bullet::mRenderer = NULL;

//------------------------------------------------------------------------------------------------
Bullet::Bullet(float x, float y, float px, float py, float angle, float speed, int damage, Person *parent)
{
	mRenderer = JRenderer::GetInstance();
	Reset(x,y,px,py,angle,speed,damage,parent);
}

//------------------------------------------------------------------------------------------------
Bullet::~Bullet()
{
}


//------------------------------------------------------------------------------------------------
void Bullet::Update(float dt)
{
	if (mState != 0) {
		mState++;
		if (mState == BULLET_DEAD) mState = BULLET_DEAD;
		return;
	}
	if (mIsFirstUpdate) {
		mIsFirstUpdate = false;
	}
	else {
		pX = mX;
		pY = mY;
	}
	//use a ray to prevent missing a collision? (nvm fixed)
	mX += cosAngle*mSpeed*dt;//cosf(mAngle)*mSpeed*dt;
	mY += sinAngle*mSpeed*dt;//sinf(mAngle)*mSpeed*dt;

}



//------------------------------------------------------------------------------------------------
void Bullet::Render(float x, float y)
{
	float offsetX = (x-SCREEN_WIDTH_2);
	float offsetY = (y-SCREEN_HEIGHT_2);

	float minX = (mSpeed*2.5f)*7*cosAngle;
	float minY = (mSpeed*2.5f)*7*sinAngle;

	mRenderer->SetTexBlend(BLEND_SRC_ALPHA, BLEND_ONE);
	int signX = (cosAngle > 0) ? 1:-1;
	int signY = (sinAngle > 0) ? 1:-1;

	float limitedX4 = limit(mX - 4 * minX, mStartX, signX) - offsetX;
	float limitedY4 = limit(mY - 4 * minY, mStartY, signY) - offsetY;
	float limitedX3 = limit(mX - 3 * minX, mStartX, signX) - offsetX;
	float limitedY3 = limit(mY - 3 * minY, mStartY, signY) - offsetY;
	float limitedX2 = limit(mX - 2 * minX, mStartX, signX) - offsetX;
	float limitedY2 = limit(mY - 2 * minY, mStartY, signY) - offsetY;
	float limitedX1 = limit(mX - minX, mStartX, signX) - offsetX;
	float limitedY1 = limit(mY - minY, mStartY, signY) - offsetY;

	float currentX = mX - offsetX;
	float currentY = mY - offsetY;
	float endX = mEndX - offsetX;
	float endY = mEndY - offsetY;

	if (mState == 0) {
		mRenderer->DrawLine(limitedX4, limitedY4, currentX, currentY, 1.0f, ARGB(75, 100, 100, 100));
		mRenderer->DrawLine(limitedX3, limitedY3, currentX, currentY, 1.1f, ARGB(125, 100, 100, 100));
		mRenderer->DrawLine(limitedX1, limitedY1, currentX, currentY, 1.25f, ARGB(225, 100, 100, 100));

		mRenderer->SetTexBlend(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
		mRenderer->DrawLine(limitedX3, limitedY3, currentX, currentY, ARGB(75, 255, 240, 0));
		mRenderer->DrawLine(limitedX2, limitedY2, currentX, currentY, ARGB(125, 255, 240, 0));
		mRenderer->DrawLine(limitedX1, limitedY1, currentX, currentY, ARGB(225, 255, 240, 0));

		mRenderer->FillRect(currentX, currentY - 1, 1, 1, ARGB(255, 255, 240, 0));
	} else {
		mRenderer->DrawLine(limitedX4, limitedY4, endX, endY, 1.0f, ARGB(75, 100, 100, 100));
		mRenderer->DrawLine(limitedX3, limitedY3, endX, endY, 1.1f, ARGB(125, 100, 100, 100));
		mRenderer->DrawLine(limitedX1, limitedY1, endX, endY, 1.25f, ARGB(225, 100, 100, 100));

		mRenderer->SetTexBlend(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
		mRenderer->DrawLine(limitedX3, limitedY3, endX, endY, ARGB(75, 255, 240, 0));
		mRenderer->DrawLine(limitedX2, limitedY2, endX, endY, ARGB(125, 255, 240, 0));
		mRenderer->DrawLine(limitedX1, limitedY1, endX, endY, ARGB(225, 255, 240, 0));

		mRenderer->FillRect(endX, endY - 1, 1, 1, ARGB(255, 255, 240, 0));

		/*float sign = (cosAngle > 0) ? 1:-1;
		if (sign*(eX-(mX-4*minX)) > 0) {
			mRenderer->DrawLine((mX-4*minX)-offsetX,(mY-4*minY)-offsetY,eX-offsetX, eY-offsetY, 1.0f, ARGB(75,100,100,100));
		}
		if (sign*(eX-(mX-3*minX)) > 0) {
			mRenderer->DrawLine((mX-3*minX)-offsetX,(mY-3*minY)-offsetY,eX-offsetX, eY-offsetY, 1.1f, ARGB(125,100,100,100));
		}
		if (sign*(eX-(mX-minX)) > 0) {
			mRenderer->DrawLine((mX-minX)-offsetX,(mY-minY)-offsetY,eX-offsetX, eY-offsetY, 1.25f, ARGB(225,100,100,100));
		}
		mRenderer->SetTexBlend(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);

		if (sign*(eX-(mX-3*minX)) > 0) {
			mRenderer->DrawLine((mX-3*minX)-offsetX,(mY-3*minY)-offsetY,eX-offsetX, eY-offsetY, ARGB(75,255,240,0));
		}
		if (sign*(eX-(mX-2*minX)) > 0) {
			mRenderer->DrawLine((mX-2*minX)-offsetX,(mY-2*minY)-offsetY,eX-offsetX, eY-offsetY, ARGB(125,255,240,0));
		}
		if (sign*(eX-(mX-minX)) > 0) {
			mRenderer->DrawLine((mX-minX)-offsetX,(mY-minY)-offsetY,eX-offsetX, eY-offsetY, ARGB(225,255,240,0));
		}
		mRenderer->FillRect(eX-offsetX, eY-offsetY-1, 1, 1, ARGB(255,255,240,0));*/
	}
}


//------------------------------------------------------------------------------------------------
void Bullet::SetAngle(float angle)
{
	mAngle = angle;
	cosAngle = cosf(mAngle);
	sinAngle = sinf(mAngle);
}
//------------------------------------------------------------------------------------------------
void Bullet::Reset(float x, float y, float px, float py, float angle, float speed, int damage, Person *parent)
{
	mState = 0;
	mX = x;
	mY = y;
	pX = px;
	pY = py;
	mStartX = px;
	mStartY = py;
	mEndX = px;
	mEndY = py;
	mAngle = angle;
	mSpeed = speed;
	mDamage = damage;
	mParent = parent;
	mParentGun = parent->mGuns[parent->mGunIndex]->mGun;
	mId = -1;
	mIsFirstUpdate = true;
	mType = TYPE_BULLET;

	cosAngle = cosf(mAngle);
	sinAngle = sinf(mAngle);
}
