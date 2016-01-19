#include "OpenGLRenderer.hpp"


int OpenGLRenderer::LoadTexture( const std::string& imageFilePath , Vec2i m_size )
{
	int openglTextureID = 0;

	int numComponents = 0; // Filled in for us to indicate how many color/alpha components the image had (e.g. 3=RGB, 4=RGBA)
	int numComponentsRequested = 0; // don't care; we support 3 (RGB) or 4 (RGBA)
	unsigned char* imageData = stbi_load( imageFilePath.c_str(), &m_size.x, &m_size.y, &numComponents, numComponentsRequested );

	// Enable texturing
	glEnable( GL_TEXTURE_2D );

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	// Ask OpenGL for an unused texName (ID number) to use for this texture
	glGenTextures( 1, (GLuint*) &openglTextureID );

	// Tell OpenGL to bind (set) this as the currently active texture
	glBindTexture( GL_TEXTURE_2D, openglTextureID );

	// Set texture clamp vs. wrap (repeat)
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); // one of: GL_CLAMP_TO_EDGE, GL_REPEAT, GL_MIRRORED_REPEAT, GL_MIRROR_CLAMP_TO_EDGE, ...
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ); // one of: GL_CLAMP_TO_EDGE, GL_REPEAT, GL_MIRRORED_REPEAT, GL_MIRROR_CLAMP_TO_EDGE, ...

	// Set magnification (texel > pixel) and minification (texel < pixel) filters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); // one of: GL_NEAREST, GL_LINEAR
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ); // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is currently in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	if( numComponents == 3 )
		bufferFormat = GL_RGB;

	// Todo: What happens if numComponents is neither 3 nor 4?

	GLenum internalFormat = bufferFormat; // the format we want the texture to me on the card; allows us to translate into a different texture format as we upload to OpenGL

	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
		GL_TEXTURE_2D,		// Creating this as a 2d texture
		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
		internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
		m_size.x,			// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,10], and B is the border thickness [0,1]
		m_size.y,			// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,10], and B is the border thickness [0,1]
		0,					// Border size, in texels (must be 0 or 1)
		bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
		GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color/alpha channel)
		imageData );		// Location of the actual pixel data bytes/buffer

	stbi_image_free( imageData );

	return openglTextureID;
}


void OpenGLRenderer::ClearScreen( float r , float g , float b )
{
	glClearColor( r, g, b, 1.f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(1.0f);
};


void OpenGLRenderer::DrawPoint(Vec3f position , Vec3f color)
{
	glPointSize(10.0f);
	glBegin(GL_POINTS);
	glColor3f( color.x , color.y , color.z );
	glVertex3f(position.x,position.y,position.z);
	glColor3f(1.0f,1.0f,1.0f);
	glEnd();
};


void OpenGLRenderer::RenderBox(Vec3f position , Vec3f size , float length, Vec3f color, Vec3f rotationDegrees)
{
	glColor3f(color.x,color.y,color.z);
	glPushMatrix();
	glTranslatef(position.x,position.y,position.z);
	glRotatef(rotationDegrees.z,0,0,1);
	glRotatef(rotationDegrees.y,0,1,0);
	glBegin(GL_QUADS);
	for(float index = 0.0f; index < length; index += 0.1f)
	{
		// bottom
		glVertex3f( -length , 0.05f  , -0.05f );
		glVertex3f(   0.05f	, 0.05f  , -0.05f );
		glVertex3f(   0.05f	, -0.05f , -0.05f );
		glVertex3f( -length , -0.05f , -0.05f );

		// top
		glVertex3f( -length , -0.05f , 0.05f );
		glVertex3f(	  0.05f	, -0.05f , 0.05f );
		glVertex3f(   0.05f	, 0.05f  , 0.05f );
		glVertex3f( -length , 0.05f  , 0.05f );

		// north
		glVertex3f( -length	, 0.05f , 0.05f  );
		glVertex3f( 0.05f	, 0.05f , 0.05f  );
		glVertex3f( 0.05f	, 0.05f , -0.05f );
		glVertex3f( -length	, 0.05f , -0.05f );

		// east
		glVertex3f( 0.05f   , 0.05f  , 0.05f  );
		glVertex3f( 0.05f	, -0.05f , 0.05f  );
		glVertex3f( 0.05f	, -0.05f , -0.05f );
		glVertex3f( 0.05f	, 0.05f  , -0.05f );

		// south
		glVertex3f( 0.05f	, -0.05f , 0.05f  );
		glVertex3f( -length	, -0.05f , 0.05f  );
		glVertex3f( -length	, -0.05f , -0.05f );
		glVertex3f( 0.05f	, -0.05f , -0.05f );

		// west
		glVertex3f( -length , -0.05f ,	0.05f  );
		glVertex3f( -length , 0.05f	,	0.05f  );
		glVertex3f( -length , 0.05f	,	-0.05f );
		glVertex3f( -length , -0.05f ,	-0.05f );
	}

	glEnd();
	glPopMatrix();
	glColor3f(1.0f,1.0f,1.0f);
}


void OpenGLRenderer::DrawLine2D(Vec2f startPoint,Vec2f endPoint,Vec3f color)
{
	glColor3f(color.x,color.y,color.z);
	glBegin(GL_LINES);	
		glVertex2f(startPoint.x,startPoint.y);
		glVertex2f(endPoint.x,endPoint.y);
	glEnd();
	glColor3f(1.0f,1.0f,1.0f);
}


void OpenGLRenderer::DrawLineLoop2D(const std::vector<Vec2f>& pointList,Vec3f color)
{
	glColor3f(color.x,color.y,color.z);
	glBegin(GL_LINE_LOOP);
	for( size_t index = 0; index < pointList.size(); index++)
		glVertex2f( pointList[index].x	, pointList[index].y );
	glEnd();
}


void OpenGLRenderer::DrawTexture2D(Vec2f position, Vec2f textureCoords, Vec2f textureSize, Vec2f drawingSize , Vec3f color)
{
	glColor3f(color.x,color.y,color.z);

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2f( textureCoords.x , textureCoords.y );
		glVertex2f( position.x , position.y + drawingSize.y );

		glTexCoord2f( textureCoords.x , textureCoords.y + textureSize.y );
		glVertex2f( position.x , position.y );

		glTexCoord2f( textureCoords.x + textureSize.x , textureCoords.y + textureSize.y );
		glVertex2f( position.x + drawingSize.x, position.y );

		glTexCoord2f( textureCoords.x + textureSize.x , textureCoords.y );
		glVertex2f( position.x + drawingSize.x, position.y + drawingSize.y );
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glColor3f(1.0f,1.0f,1.0f);
}


void OpenGLRenderer::ChangeTexture(int textureID)
{
	glBindTexture(GL_TEXTURE_2D, textureID);
}


void OpenGLRenderer::DrawTexture3D(const std::vector<Vec3f>& pointList, Vec2f textureCoords, Vec2f textureSize, Vec3f color)
{
	glColor3f(color.x,color.y,color.z);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

		glTexCoord2f( textureCoords.x , textureCoords.y );
		glVertex3f(pointList[0].x,pointList[0].y,pointList[0].z);
	
		glTexCoord2f( textureCoords.x + textureSize.x , textureCoords.y );
		glVertex3f(pointList[1].x,pointList[1].y,pointList[1].z);
	
		glTexCoord2f( textureCoords.x + textureSize.x , textureCoords.y + textureSize.y );
		glVertex3f(pointList[2].x,pointList[2].y,pointList[2].z);
		
		glTexCoord2f( textureCoords.x , textureCoords.y + textureSize.y );
		glVertex3f(pointList[3].x,pointList[3].y,pointList[3].z);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f,1.0f,1.0f);
}