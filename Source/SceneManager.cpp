///////////////////////////////////////////////////////////////////////////////
// shadermanager.cpp
// ============
// manage the loading and rendering of 3D scenes
//
//  AUTHOR: Nii Amatey Tagoe - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager *pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.ambientColor = m_objectMaterials[index].ambientColor;
			material.ambientStrength = m_objectMaterials[index].ambientStrength;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationX * rotationY * rotationZ * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor);
			m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/

/**************************************************************/
/*** The code in the methods BELOW is for preparing and     ***/
/*** rendering the 3D replicated scenes.                    ***/
/**************************************************************/

/***********************************************************
 *  LoadSceneTextures()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene
 *  rendering
 ***********************************************************/
void SceneManager::LoadSceneTextures()
{
	bool bReturn = false;

	bReturn = CreateGLTexture(
		"../../Utilities/textures/stainless.jpg",
		"stand");

	bReturn = CreateGLTexture(
		"../../Utilities/textures/cheese_wheel.jpg",
		"mug");

	bReturn = CreateGLTexture(
		"../../Utilities/textures/drywall.jpg",
		"screen");

	bReturn = CreateGLTexture(
		"../../Utilities/textures/tilesf2.jpg",
		"handle");

	bReturn = CreateGLTexture(
		"../../Utilities/textures/stainless_end.jpg",
		"base");

	bReturn = CreateGLTexture(
		"../../Utilities/textures/knife_handle.jpg",
		"plane");

	// after the texture image data is loaded into memory, the
	// loaded textures need to be bound to texture slots - there
	// are a total of 16 available slots for scene textures
	BindGLTextures();
}
void SceneManager::DefineObjectMaterials()
{
	OBJECT_MATERIAL mugMaterial;
	mugMaterial.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
	mugMaterial.ambientStrength = 0.2f;
	mugMaterial.diffuseColor = glm::vec3(0.5f, 0.5f, 0.5f);
	mugMaterial.specularColor = glm::vec3(0.1f, 0.1f, 0.1f);
	mugMaterial.shininess = 0.3;
	mugMaterial.tag = "cheesy";

	m_objectMaterials.push_back(mugMaterial);

	OBJECT_MATERIAL screenMaterial;
	screenMaterial.ambientColor = glm::vec3(0.4f, 0.4f, 0.4f);
	screenMaterial.ambientStrength = 0.3f;
	screenMaterial.diffuseColor = glm::vec3(0.3f, 0.3f, 0.3f);
	screenMaterial.specularColor = glm::vec3(0.6f, 0.6f, 0.6f);
	screenMaterial.shininess = 85.0;
	screenMaterial.tag = "glassy";

	m_objectMaterials.push_back(screenMaterial);

	OBJECT_MATERIAL standMaterial;
	standMaterial.ambientColor = glm::vec3(0.2f, 0.2f, 0.2f);
	standMaterial.ambientStrength = 0.3f;
	standMaterial.diffuseColor = glm::vec3(0.2f, 0.2f, 0.2f);
	standMaterial.specularColor = glm::vec3(0.5f, 0.5f, 0.5f);
	standMaterial.shininess = 22.0;
	standMaterial.tag = "metal";

	m_objectMaterials.push_back(standMaterial);

	OBJECT_MATERIAL baseMaterial;
	baseMaterial.ambientColor = glm::vec3(0.2f, 0.2f, 0.2f);
	baseMaterial.ambientStrength = 0.3f;
	baseMaterial.diffuseColor = glm::vec3(0.2f, 0.2f, 0.2f);
	baseMaterial.specularColor = glm::vec3(0.5f, 0.5f, 0.5f);
	baseMaterial.shininess = 22.0;
	baseMaterial.tag = "shiny";

	m_objectMaterials.push_back(baseMaterial);

	OBJECT_MATERIAL woodMaterial;
	woodMaterial.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
	woodMaterial.ambientStrength = 0.2f;
	woodMaterial.diffuseColor = glm::vec3(0.3f, 0.3f, 0.3f);
	woodMaterial.specularColor = glm::vec3(0.1f, 0.1f, 0.1f);
	woodMaterial.shininess = 0.3;
	woodMaterial.tag = "wood";

	m_objectMaterials.push_back(woodMaterial); 
}

/***********************************************************
 *  SetupSceneLights()
 *
 *  This method is called to add and configure the light
 *  sources for the 3D scene.  There are up to 4 light sources.
 ***********************************************************/
void SceneManager::SetupSceneLights()
{
	// this line of code is NEEDED for telling the shaders to render 
	// the 3D scene with custom lighting - to use the default rendered 
	// lighting then comment out the following line
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setBoolValue(g_UseLightingName, true);

		// Point light setup
		m_pShaderManager->setVec3Value("lightSources[0].position", glm::vec3(3.0f, 5.0f, 3.0f));
		m_pShaderManager->setVec3Value("lightSources[0].ambientColor", glm::vec3(0.6f, 0.6f, 0.9f)); // Blue ambient light
		m_pShaderManager->setVec3Value("lightSources[0].diffuseColor", glm::vec3(0.4f, 0.4f, 1.0f));
		m_pShaderManager->setVec3Value("lightSources[0].specularColor", glm::vec3(1.0f, 1.0f, 1.0f));
		m_pShaderManager->setFloatValue("lightSources[0].focalStrength", 32.0f);
		m_pShaderManager->setFloatValue("lightSources[0].specularIntensity", 1.0f);

		// Directional light setup
		m_pShaderManager->setVec3Value("lightSources[1].position", glm::vec3(3.0f, 5.0f, -5.0f));
		m_pShaderManager->setVec3Value("lightSources[1].ambientColor", glm::vec3(0.3f, 0.3f, 0.3f)); // White ambient light
		m_pShaderManager->setVec3Value("lightSources[1].diffuseColor", glm::vec3(1.0f, 0.9f, 0.7f));
		m_pShaderManager->setVec3Value("lightSources[1].specularColor", glm::vec3(1.0f, 1.0f, 1.0f));
		m_pShaderManager->setFloatValue("lightSources[1].focalStrength", 16.0f);
		m_pShaderManager->setFloatValue("lightSources[1].specularIntensity", 0.8f);
	}
}

/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	LoadSceneTextures();
	SetupSceneLights();
	DefineObjectMaterials();

	// only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene
	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadTorusMesh();
	m_basicMeshes->LoadTaperedCylinderMesh();
	m_basicMeshes->LoadSphereMesh();
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by 
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
	RenderPlane();
	RenderMugBody();
	RenderMugHandle();
	RenderMonitorScreen();
	RenderMonitorBase();
	RenderMonitorStand();
	RenderMouse();
	RenderKeyboard();
	RenderBook1();
	RenderBook2();
}

void SceneManager::RenderPlane()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(50.0f, 1.0f, 50.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, -1.0f, 0.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	//SetShaderColor(0.6f, 0.6f, 0.6f, 1.0f);
	SetShaderMaterial("wood");
	SetShaderTexture("plane");
	SetTextureUVScale(10.0f, 10.0f);
	// draw the mesh with transformation values
	m_basicMeshes->DrawPlaneMesh();
}


	/****************************************************************/
	/*** Render the upside-down tapered cylinder ***/
	// Set scale for the tapered cylinder
void SceneManager::RenderMugBody()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.0f, 1.5f, 1.0f); // Scale for height and radius

	// Set rotation to flip the cylinder upside down
	XrotationDegrees = 180.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Set position for the cylinder
	positionXYZ = glm::vec3(3.0f, 0.5f, 3.0f); // Slightly above the origin

	// Apply transformations and set shader color
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	//SetShaderColor(0.7f, 0.4f, 0.1f, 1.0f); // Set a brown color for the mug body
	SetShaderTexture("mug");
	SetShaderMaterial("glassy");
	SetTextureUVScale(2.25f, 2.25f);

	// Draw the tapered cylinder
	m_basicMeshes->DrawTaperedCylinderMesh();
}

void SceneManager::RenderMugHandle()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/

	/*** Render the torus for the handle ***/
	// Set scale for the torus
	scaleXYZ = glm::vec3(0.5f, 0.4f, 0.5f); // Uniform scaling for the handle

	// Set rotation to orient the torus as a handle
	XrotationDegrees = 0.0f; // Rotate to align with the mug body
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Set position for the torus (beside the tapered cylinder)
	positionXYZ = glm::vec3(4.0f, -0.1f, 3.25f);

	// Apply transformations and set shader color
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	//SetShaderColor(0.5f, 0.8f, 1.0f, 1.0f); // applying texture to the handle of the mug
	SetShaderTexture("handle");
	SetShaderMaterial("glassy");
	SetTextureUVScale(1.0f, 1.0f);

	// Draw the torus
	m_basicMeshes->DrawTorusMesh();
}
void SceneManager::RenderMonitorScreen()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/

	/*** Render the computer monitor screen ***/
	// Set scale for the BoxMesh
	scaleXYZ = glm::vec3(9.0f, 4.0f, 0.2f); // Uniform scaling 

	// Set rotation to orient the BoxMesh as the monitor screen
	XrotationDegrees = 0.0f; // Rotate to align
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Set position for the BoxMesh at the top of the scene
	positionXYZ = glm::vec3(0.0f, 3.0f, 0.0f);

	// Apply transformations and set shader color
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	//SetShaderColor(0.5f, 0.8f, 1.0f, 1.0f);
	SetShaderTexture("screen");
	SetShaderMaterial("glassy");
	SetTextureUVScale(1.0f, 1.0f);

	// Draw the BoxMesh
	m_basicMeshes->DrawBoxMesh();
}
void SceneManager::RenderMonitorStand()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	/*** Render the monitor stand ***/
	// Set scale for the BoxMesh
	scaleXYZ = glm::vec3(1.0f, 0.5f, 1.5f); // Uniform scaling 

	// Set rotation to orient the BoxMesh as the monitor stand
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Set position for the BoxMesh at the top of the scene
	positionXYZ = glm::vec3(0.0f, 0.75f, 0.0f);

	// Apply transformations and set shader color
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	//SetShaderColor(0.5f, 0.8f, 1.0f, 1.0f);
	SetShaderTexture("stand");
	SetShaderMaterial("metal");
	SetTextureUVScale(4.0f, 4.0f);

	// Draw the BoxMesh
	m_basicMeshes->DrawBoxMesh();
}
void SceneManager::RenderMonitorBase()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	/*** Render the computer monitor base ***/
	// Set scale for the TaperedCylinderMesh
	scaleXYZ = glm::vec3(1.25f, 0.2f, 2.0f); // Uniform scaling 

	// Set rotation to orient the TaperedCylinderMesh as the base
	XrotationDegrees = 0.0f; // Rotate to align
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Set position for the TaperedCylinderMesh
	positionXYZ = glm::vec3(0.0f, 0.5f, 0.0f);

	// Apply transformations and set shader color
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	//SetShaderColor(0.7f, 0.4f, 0.1f, 1.0f);
	SetShaderTexture("base");
	SetShaderMaterial("metal");
	SetTextureUVScale(5.0f, 5.0f);

	// Draw the TaperedCylinderMesh
	m_basicMeshes->DrawTaperedCylinderMesh();
}
void SceneManager::RenderBook1()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	/*** Render the Book 1 ***/
	// Set scale for the BoxMesh
	scaleXYZ = glm::vec3(2.0f, 0.5f, 3.0f); // Uniform scaling 

	// Set rotation to orient the BoxMesh as a book
	XrotationDegrees = 0.0f; // Rotate to align
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Set position for the BoxMesh
	positionXYZ = glm::vec3(-6.0f, 0.5f, 1.0f);

	// Apply transformations and set shader color
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderColor(0.5f, 0.8f, 1.0f, 1.0f);
	//SetShaderTexture("handle");
	//SetShaderMaterial("");

	// Draw the BoxMesh
	m_basicMeshes->DrawBoxMesh();
}
void SceneManager::RenderBook2()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/

	/*** Render the Book 2 ***/
	// Set scale for the BoxMesh
	scaleXYZ = glm::vec3(2.0f, 0.5f, 3.0f); // Uniform scaling 

	// Set rotation to orient the BoxMesh as a book
	XrotationDegrees = 0.0f; // Rotate to align
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Set position for the BoxMesh
	positionXYZ = glm::vec3(-6.0f, 1.0f, 1.0f);

	// Apply transformations and set shader color
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderColor(0.7f, 0.4f, 0.1f, 1.0f);
	//SetShaderTexture("");
	//SetShaderMaterial("");
	//SetTextureUVScale("");

	// Draw the BoxMesh
	m_basicMeshes->DrawBoxMesh();
}
void SceneManager::RenderKeyboard()
{ 	
// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/

	/*** Render the KeyBoard ***/
	// Set scale for the BoxMesh
	scaleXYZ = glm::vec3(7.0f, 0.2f, 1.0f); // Uniform scaling 

	// Set rotation to orient the BoxMesh as the keyboard
	XrotationDegrees = 0.0f; // Rotate to align
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Set position for the BoxMesh
	positionXYZ = glm::vec3(0.0f, 0.2f, 2.0f);

	// Apply transformations and set shader color
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderColor(0.7f, 0.4f, 0.1f, 1.0f);
	//SetShaderTexture("");
	//SetShaderMaterial("");

	// Draw the BoxMesh
	m_basicMeshes->DrawBoxMesh();
}
void SceneManager::RenderMouse()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	/*** Render the Mouse ***/
	// Set scale for the SphereMesh
	scaleXYZ = glm::vec3(0.5f, 0.3f, 0.8f); // Uniform scaling 

	// Set rotation to orient the SphereMesh as the Mouse
	XrotationDegrees = 0.0f; // Rotate to align
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Set position for the SphereMesh
	positionXYZ = glm::vec3(-3.0f, -0.5f, 2.5f);

	// Apply transformations and set shader color
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderColor(0.7f, 0.4f, 0.1f, 1.0f);
	//SetShaderTexture("");
	//SetShaderMaterial("");

	// Draw the SphereMesh
	m_basicMeshes->DrawSphereMesh();
}

