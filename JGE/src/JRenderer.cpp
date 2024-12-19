#include "../include/JGE.h"
#include "../include/JRenderer.h"
#include "../include/JResourceManager.h"

JQuad::JQuad(JTexture *tex, float x, float y, float width, float height)
		:mTex(tex), mX(x), mY(y), mWidth(width), mHeight(height)
{

	assert(tex != NULL);

	mHotSpotX = 0.0f;
	mHotSpotY = 0.0f;
		
	mColor.color = 0xFFFFFFFF;

	mHFlipped = false;
	mVFlipped = false;

	SetTextureRect(x, y, width, height);

}

void JQuad::SetTextureRect(float x, float y, float w, float h) 
{ 
	mX = x;
	mY = y;
	mWidth = w;
	mHeight = h;
}

void JQuad::SetColor(PIXEL_TYPE color)
{
	mColor.color = color;
}



void JQuad::SetHotSpot(float x, float y)
{
	mHotSpotX = x;
	mHotSpotY = y;
}


//////////////////////////////////////////////////////////////////////////

JTexture::JTexture()
{
	mTexId = -1;
}

JTexture::~JTexture()
{
	if (mTexId != -1)
		glDeleteTextures(1, &mTexId);
}

void JTexture::UpdateBits(int width, int height, PIXEL_TYPE* bits)
{
	glBindTexture(GL_TEXTURE_2D, mTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);
}

//////////////////////////////////////////////////////////////////////////

JRenderer* JRenderer::mInstance = NULL;

JRenderer* JRenderer::GetInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new JRenderer();

		assert(mInstance != NULL);

		mInstance->InitRenderer();
	}
	
	return mInstance;
}

void JRenderer::InitRenderer()
{
    mCurrentTextureFilter = TEX_FILTER_NONE;

    mCurrTexBlendSrc = BLEND_SRC_ALPHA;
    mCurrTexBlendDest = BLEND_ONE_MINUS_SRC_ALPHA;

    // Load shaders
    JResourceManager::LoadShader("sprite.vert", "sprite.frag", nullptr, "sprite");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(SCREEN_WIDTH_F),
        static_cast<GLfloat>(SCREEN_HEIGHT_F), 0.0f, -1.0f, 1.0f);
    JShader spriteShader = JResourceManager::GetShader("sprite");
    spriteShader.Use();
    spriteShader.SetInteger("image", 0);
    spriteShader.SetMatrix4("projection", projection);

    JResourceManager::LoadShader("simple.vert", "simple.frag", nullptr, "simple");
    JShader simpleShader = JResourceManager::GetShader("simple");
    simpleShader.Use();
    simpleShader.SetMatrix4("projection", projection);
    colorUniformLoc = glGetUniformLocation(simpleShader.Program, "color");

    // Load sprite renderer
    mSpriteRenderer = new JSpriteRenderer(spriteShader);

    // Load Vertex Buffer Object
    JRenderer::InitVBO();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void JRenderer::Destroy()
{
	if (mInstance)
	{
		mInstance->DestroyRenderer();
		delete mInstance;
		mInstance = NULL;
	}
}

JRenderer::JRenderer()
{
}


JRenderer::~JRenderer()
{
}

void JRenderer::InitVBO()
{
    bufferSize = 360;
    elementBufferSize = 360;

    JShader shader = JResourceManager::GetShader("simple").Use();

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, bufferSize * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

    GLint vertexLocation = glGetAttribLocation(shader.Program, "vertex");
    glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(vertexLocation);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void JRenderer::DestroyRenderer()
{
	JResourceManager::Clear();
}

void JRenderer::BeginScene()
{
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
}


void JRenderer::EndScene()
{
	// glFlush ();
}

void JRenderer::EnableTextureFilter(bool flag)
{
	if (flag)
		mCurrentTextureFilter = TEX_FILTER_LINEAR;
	else
		mCurrentTextureFilter = TEX_FILTER_NEAREST;
}

JTexture* JRenderer::CreateTexture(int width, int height )
{
	int size = width * height * sizeof(PIXEL_TYPE);			// RGBA

	u8* buffer = new u8[size];

	JTexture *tex = new JTexture();

	if (buffer && tex)
	{
		tex->mWidth = width;
		tex->mHeight = height;
		tex->mTexWidth = width;
		tex->mTexHeight = height;

		GLuint texid; 
		glGenTextures(1, &texid);
		tex->mTexId = texid;

		memset(buffer, 0, size);

		glBindTexture(GL_TEXTURE_2D, texid);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

		delete buffer;

		return tex;
	}
	else
		return NULL;
}

void JRenderer::ClearScreen(PIXEL_TYPE color)
{
	JColor col;
	col.color = color;
	glClearColor(col.r / 255.f, 
				col.g / 255.f, 
				col.b / 255.f, 
				col.a / 255.f);
	glClear (GL_COLOR_BUFFER_BIT);
}

void JRenderer::SetTexBlend(int src, int dest)
{
	if (src != mCurrTexBlendSrc || dest != mCurrTexBlendDest)
	{
		mCurrTexBlendSrc = src;
		mCurrTexBlendDest = dest;
		
		glBlendFunc(src, dest);
	}
}


void JRenderer::SetTexBlendSrc(int src)
{
	if (src != mCurrTexBlendSrc)
	{
		mCurrTexBlendSrc = src;
		glBlendFunc(mCurrTexBlendSrc, mCurrTexBlendDest);
	}
}


void JRenderer::SetTexBlendDest(int dest)
{
	if (dest != mCurrTexBlendDest)
	{
		mCurrTexBlendDest = dest;
		glBlendFunc(mCurrTexBlendSrc, mCurrTexBlendDest);
	}
}


void JRenderer::Enable2D()
{
	glDisable (GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
}

void JRenderer::RenderQuad(JQuad* quad, float xo, float yo, float angle, float xScale, float yScale)
{
    static JSprite sprite;

    const float colorNormalization = 1.0f / 255.0f;

    sprite.texture = quad->mTex;
    sprite.spriteRect = {quad->mX, quad->mY, quad->mWidth, quad->mHeight};
    sprite.position = {xo, yo};
    sprite.hotspot = {quad->mHotSpotX, quad->mHotSpotY};
    sprite.scale = {xScale, yScale};
    sprite.rotate = angle;
    sprite.hFlipped = quad->mHFlipped;
    sprite.vFlipped = quad->mVFlipped;
    sprite.color = glm::vec4(quad->mColor.r, quad->mColor.g, quad->mColor.b, quad->mColor.a) * colorNormalization;
    sprite.textureFilter = mCurrentTextureFilter;

    mSpriteRenderer->DrawSprite(sprite);
}

// void JRenderer::RenderQuadBatch(JQuad* quad, float xo, float yo, float angle, float xScale, float yScale)
// {
//     static JSprite sprite;

//     const float colorNormalization = 1.0f / 255.0f;

//     sprite.texture = quad->mTex;
//     sprite.spriteRect = {quad->mX, quad->mY, quad->mWidth, quad->mHeight};
//     sprite.position = {xo, yo};
//     sprite.hotspot = {quad->mHotSpotX, quad->mHotSpotY};
//     sprite.scale = {xScale, yScale};
//     sprite.rotate = angle;
//     sprite.hFlipped = quad->mHFlipped;
//     sprite.vFlipped = quad->mVFlipped;
//     sprite.color = glm::vec4(quad->mColor.r, quad->mColor.g, quad->mColor.b, quad->mColor.a) * colorNormalization;
//     sprite.textureFilter = mCurrentTextureFilter;

//     //mSpriteRenderer->DrawSprite(sprite);
// 	mSpriteBatch.push_back(sprite);
// }

void JRenderer::DrawPolygon(float* x, float* y, int count, PIXEL_TYPE color, GLenum mode)
{
    JShader shader = JResourceManager::GetShader("simple").Use();

    int buf_size = 2 * count; // 2 coordinates per vertex
    GLfloat vertices[buf_size]; 
    for (int i = 0; i < count; i++) {
        vertices[2 * i] = x[i];
        vertices[2 * i + 1] = y[i];
    }

    if (buf_size > bufferSize) {
        printf("Vertex buffer too small! \n");
    }

    JColor col;
    col.color = color;
    glUniform4f(colorUniformLoc, 
                col.r / 255.f, 
                col.g / 255.f, 
                col.b / 255.f, 
                col.a / 255.f);
    
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, buf_size * sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW);
    
    GLint vertexLocation = glGetAttribLocation(shader.Program, "vertex");
    glEnableVertexAttribArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(mode, 0, count);

    glDisableVertexAttribArray(vertexLocation);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void JRenderer::DrawPolygon(float x, float y, float size, int count, float startingAngle, PIXEL_TYPE color, GLenum mode)
{
	float angle = -startingAngle*RAD2DEG;
	float steps = 360.0f/count;

	float vertices_x[count];
	float vertices_y[count];

	for(int i=0; i<count;i++)
	{
		vertices_x[i] = x+size*COSF((int)angle);
		vertices_y[i] = y+size*SINF((int)angle);

		angle += steps;
		if (angle >= 360.0f)
			angle -= 360.0f;
	}

	DrawPolygon(vertices_x, vertices_y, count, color, mode);
}

void JRenderer::FillRect(float x, float y, float width, float height, PIXEL_TYPE color)
{
	float vertices_x[] = {x, x+width, x+width, x};
	float vertices_y[] = {y, y, y+height, y+height};
	FillPolygon(vertices_x, vertices_y, 4, color);
}


void JRenderer::DrawRect(float x, float y, float width, float height, PIXEL_TYPE color)
{
	float vertices_x[] = {x, x+width, x+width, x};
	float vertices_y[] = {y, y, y+height, y+height};
	DrawPolygon(vertices_x, vertices_y, 4, color);
}

void JRenderer::DrawLine(float x1, float y1, float x2, float y2, PIXEL_TYPE color)
{
    GLfloat vertices[] = { x1, y1, x2, y2 };
    glLineWidth(2.0f);

    JShader shader = JResourceManager::GetShader("simple").Use();

    JColor col;
    col.color = color;
    glUniform4f(colorUniformLoc, 
                col.r / 255.f, 
                col.g / 255.f, 
                col.b / 255.f, 
                col.a / 255.f);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    
    GLint vertexLocation = glGetAttribLocation(shader.Program, "vertex");
    glEnableVertexAttribArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_LINES, 0, 2);

    glDisableVertexAttribArray(vertexLocation);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void JRenderer::DrawLine(float x1, float y1, float x2, float y2, float lineWidth, PIXEL_TYPE color)
{
	float dy=y2-y1;
	float dx=x2-x1;
	if(dy==0 && dx==0)
		return;

	float l=(float)hypot(dx,dy);

	float x[4];
	float y[4];

	x[0]=x1+lineWidth*(y2-y1)/l;
	y[0]=y1-lineWidth*(x2-x1)/l;

	x[1]=x1-lineWidth*(y2-y1)/l;
	y[1]=y1+lineWidth*(x2-x1)/l;

	x[2]=x2-lineWidth*(y2-y1)/l;
	y[2]=y2+lineWidth*(x2-x1)/l;

	x[3]=x2+lineWidth*(y2-y1)/l;
	y[3]=y2-lineWidth*(x2-x1)/l;

	FillPolygon(x, y, 4, color);
}

void JRenderer::FillPolygon(float* x, float* y, int count, PIXEL_TYPE color, bool convex)
{
    if (convex) {
        JShader shader = JResourceManager::GetShader("simple").Use();
        
        int buf_size = 2 * count;
        GLfloat vertices[buf_size]; 
        for (int i = 0; i < count; i++) {
            vertices[2 * i] = x[i];
            vertices[2 * i + 1] = y[i];
        }

        JColor col;
        col.color = color;
        glUniform4f(colorUniformLoc, 
                    col.r / 255.f, 
                    col.g / 255.f, 
                    col.b / 255.f, 
                    col.a / 255.f);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, buf_size * sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW);
        
        GLint vertexLocation = glGetAttribLocation(shader.Program, "vertex");
        glEnableVertexAttribArray(vertexLocation);
        glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, count);

        glDisableVertexAttribArray(vertexLocation);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    } else {
        std::cerr << "Non-convex polygon handling is not implemented in this version." << std::endl;
    }
}

void JRenderer::FillPolygon(float x, float y, float size, int count, float startingAngle, PIXEL_TYPE color)
{
	DrawPolygon(x, y, size, count, startingAngle, color, GL_TRIANGLE_FAN);
}

void JRenderer::DrawCircle(float x, float y, float radius, PIXEL_TYPE color)
{
	DrawPolygon(x, y, radius, 180, 0, color);
}

void JRenderer::FillCircle(float x, float y, float radius, PIXEL_TYPE color)
{
	FillPolygon(x, y, radius, 180, 0, color);
}