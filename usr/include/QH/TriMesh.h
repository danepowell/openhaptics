//---------------------------------------------------------------------
//
// Copyright 2008, SensAble Technologies, Inc.
//
// File: TriMesh.cpp
//
// Author: Hari Vasudevan	
//
// Created: 09/11/2008
//
// Description: Declares the TriMesh Class primitive with all associated 
// parameters. It contains functions to load STL, PLY, 3DS and OBJ meshes.
// In addition it descibes functions for making the models deformable (experimental)
//---------------------------------------------------------------------

#ifndef TRIMESH_H
#define TRIMESH_H


#include "ExportDef.h"
#include "Shape.h"
//#include "glm.h"
//#include "ply.h"
//#include "Model_3DS.h"


/*!\brief This class to loads  a triangluar mesh.

	Use this class to load a triangular mesh. The class contains parsers for OBJ, 3DS, STL and PLY formats. 
	
	1) The class generates a display list for the mesh and then supplies display list that to the parent
	shape class.

	2) If the user wishes he/she can render the model every frame by passing the display list.

	3) Class also contains functions experimental to treat the mesh as a spring mass system and calculate deformation based on contact.

	4) Contains other search functions and data structures required for loading the model
*/


class QHAPI TriMesh: public Shape
{
	
	friend class QHRenderer;
	friend class Cursor;
	
	///Function to find the distance between any two point in space
	GLdouble distanceFinder(GLuint ParentIndex,hduVector3Dd m_Position);
	///Function to render the binary tree graphically and haptically, This function is called if the m_DynamicMeshFlag variable is true
	void render(void);
	///If a user has a a data file then this variable holds the filename.
	char* m_FileName;
	///Variable for use in calculating vertex normals.
	GLfloat m_ShadingCutoffAngle;


	
	///Vertex Structure for the Binary Tree
	struct VertexBinary
	{
		GLuint* m_pConnectionsIndex;
		GLuint m_NoConnections;

		GLuint* m_pVertexConnectionsIndex;
		GLuint m_NoVertexConnections;
		GLdouble* m_pVertexConnectionsLength;

		hduVector3Dd m_FixedVertex;
		hduVector3Dd m_Vertex;
		hduVector3Dd m_Velocity;
		hduVector3Dd m_Acceleration;
		bool m_ProcessingFlag;
		bool m_FixedFlag;

		int m_Valid;
		int m_Index;
		int m_LeftChildNodeIndex;
		int m_RightChildNodeIndex;
		GLuint m_ParentNodeIndex;
	};
	
    ///Flag for waiting for convergence for deformable models.
    bool m_DestinationStateProcessingFlag;
	///This flag controls whether the Mesh displayed had facetNormals or vertex normals
    bool m_FacetNormalsFlag;




	///PLY Parser
	bool binaryTreePLYReader(char* filename);
	///3DS Parser
	bool binaryTree3DSReader(char* filename);
	///STL Parser (Binary)
	bool binaryTreeSTLReaderBinary(char* filename);
	///STL Parser (ASCII)
	bool binaryTreeSTLReaderASCII(char *filename);
	///OBJ Parser
	void binaryTreeOBJReader(char*);
	///Function to build the Binary Tree
	bool vertexBinaryTree(hduVector3Dd m_Vertex, int i , int j, int Degenerate);
	///Function to generate vertex normals
	void binaryTreeMeshVertexNormals(void);
	///Function to generate the Display List
	void binaryTreeDisplayListGeneratorColor(void);
	///Function to find the rest lenght between adjacent vertices of a Trimesh. Use for deformable body simulations
	void restLengthCalculator(void);
	///Internally used function to find the closest vertex in a binary tree. Used without world to model transforms
	GLuint searchBinaryTreeInternal(hduVector3Dd m_Position);

    ///This function determines if the feedback or depth buffer has to be used based on the number of vertices in the model
    void checkAndSetRenderMode(void);

	///Flag used to indicate if the display list of a model should be used or the model should be redrawn each frame
	bool m_DynamicMeshFlag;

	///This flag controls if the model is drawn with vertex or facet normals
	bool m_VertexNormalsFlag;

		
	
	//The following variables are used to hold meshdata. The user might need this for rendering 
	//deformable or shape changing meshes
	
	///Number of vertices in the model
	GLuint m_NumVertices;
	///Actual pointer to a base of the actual vertex list
	GLfloat* m_pVertices;
	
	///Number of normals in the model
	GLuint m_NumNormals;
	///Actual pointer to the base of the vertex normals list
	GLdouble* Normals;

	///Number of facet normals
	GLuint m_NumFacetNormals;
	///Actual pointer to the base of the facet normals list
	GLdouble* m_pFacetNormals;
	
	///Number of normals in the model
	GLuint m_NumTextureCoordinates;
	///Actual pointer to the base of the m_Texture Coodinate list
	GLfloat* m_pTextureCoordinates;
	///Pointer to the colors array
	GLfloat* m_pFacetColors;

	///Number of vertex indices in the model
	GLuint m_NumVertexIndices;
	///Actual pointer to the vertex indices list
	GLuint* m_pVertexIndices;
	///Number of vertex normals in the model
	GLuint m_NumNormalIndices;
	///Actual pointer to the Normal Indices
	GLuint* m_pNormalIndices;
	///Number of facet Normals in the model
	GLuint m_NumFacetNormalIndices;
	///Actual pointer to the Facet Normal Indices
	GLuint* m_pFacetNormalIndices;
	///Number of facet color Indices int the model
	GLuint m_NumFacetColorsIndices;
	///Indices to the colors array
	GLuint* m_pFacetColorsIndices;


	
	///Variable to hold the TextureIDs in a Trimesh model
	GLuint* m_pTextureID;
	///Actual pointer to the m_Texture indices
	GLuint* m_pTextureIndices;
	///Variable to allocate memory for the binary tree. Specified how many nodes are required in this implaemntation of the binary tree
	GLuint m_TreeArraySize;
	///Array to hold vertex information
	VertexBinary* m_pVertexTreeArray;

	///Sometimes models are read in flipped (ie with inverted normals) This variable is used to flip the model
	bool FlipModelFlag;
	
	///m_Mass per node for a deformable mesh
	GLdouble m_Mass;
	///Spring stiffness for the spring mass system
	GLdouble m_SpringStiffness;
	///m_Damping for the spring mass system.
	GLdouble m_SpringDamping;

    ///Variable for storing the bound box
    hduBoundBox3Dd TriMeshBoundBox;

	//This flag controls whether gravity affects the deformable mesh or not.
	bool m_GravityFlag;


	//Function to flip the model. Associated with the variable "FlipModelFlag"
	void flip(void);

	///This is a helper function that can be used to get rid of the color and texture of a 3d model is rendered in the dynamic mode.
	void InternalFnSetNullColorAndTexture(void);
	///Load the mesh from file and select appropriate parser for file type
	void InternalFnLoadMesh(char* m_FileName,GLuint LoadOrigin);
	///Function to render the TriMesh graphically, This function is called if the display list is not being used. (m_DynamicMeshFlag variable is true)
	void InternalFnBinaryTreeDisplayGraphic(void);
	///Function to render the TriMesh haptically, This function is called if the display list is not being used. (m_DynamicMeshFlag variable is true)
	void InternalFnBinaryTreeDisplayHaptic(void);


	#ifdef WIN32 	// Compiling for Windows
		LARGE_INTEGER m_PreviousTimeStamp;
	#elif defined(linux)// Compiling for Linux
		struct timeval m_PreviousTime;
	#endif

    
	
	
	
	public:

	

		
	/*! \brief This function returns the state of the DynamicMeshFlag variable. 
	
	The DynamicMeshFlag variable is used to indicate if the display list of a model should be used or the model should be redrawn each frame. The flag is 
	set using the TriMesh::dynamic function*/
	bool getDynamicState(void);

	//This function returns the vertex coordinates if given the index number of the vertex in the binary tree
	hduVector3Dd getVertex(GLuint IndexNumber);
    
	/*! \brief This function returns the bound box of the Trimesh

	The boundbox returned is the two diagonal vertices of the box that fits the model. The bound box is NOT axis aligned*/
    hduBoundBox3Dd getBoundBox(void);
	
	/*! \brief This function is used to load a 3D model in to the QuickHaptics uAPI

	Supported Models are:\n
	STL, PLY, OBJ, 3DS\n
	The models will be positioned exactly as they were placed in the modelling package in which they were created*/
	TriMesh(char* m_FileName);
	/*! \brief This function is used to load a 3D model in to the QuickHaptics uAPI. The additional parameter flip turns the faces inside out.

	Supported Models are:\n
	STL, PLY, OBJ, 3DS\n
	The models will be positioned exactly as they were placed in the modelling package in which they were created*/
	TriMesh(char* m_FileName, bool flip);
	/*! \brief This function is used to load a 3D model in to the QuickHaptics uAPI. This constructor allows the programmer
	to specify the location of the model in worldspace.


	Supported Models are:\n
	STL, PLY, OBJ, 3DS\n
	The displacements m_PositionX, m_PositionY and m_PositionZ control the translation of the model's origin from the origin of the worldspace*/
	TriMesh(char* m_FileName, HLdouble scale, HLdouble m_PositionX, HLdouble m_PositionY, HLdouble m_PositionZ);
	/*! \brief This function is used to load a 3D model in to the QuickHaptics uAPI. This constructor allows the programmer
	to specify the location of the model in worldspace. The flip parameter turns the faces inside out.


	Supported Models are:\n
	STL, PLY, OBJ, 3DS\n
	The displacements m_PositionX, m_PositionY and m_PositionZ control the translation of the model's origin from the origin of the worldspace*/
	TriMesh(char* m_FileName, HLdouble scale, HLdouble m_PositionX, HLdouble m_PositionY, HLdouble m_PositionZ, bool flip);
	/*! \brief This function is used to load a 3D model in to the QuickHaptics uAPI. This constructor allows the programmer
	to specify the location of the model in worldspace and additionally the programmer can also specify the rotation of the model
	about a given axis.


	Supported Models are:\n
	STL, PLY, OBJ, 3DS\n
	The displacements m_PositionX, m_PositionY and m_PositionZ control the translation of the model's origin from the origin of the worldspace
	The programmer has to specify the angle, the axis of rotation and also if the feces have to be flipped inside out*/
	TriMesh(char* m_FileName, HLdouble scale, HLdouble m_PositionX, HLdouble m_PositionY, HLdouble m_PositionZ, HLdouble AxisX, HLdouble AxisY, HLdouble AxisZ, HLdouble m_RotationFlag, bool m_FacetNormalsFlag, bool flip);
	///Constructor with outfilename. In case the programmer uses this by accident, no exception should be generated. the constructor properly initialises all variables to prevent any exceptions.
	TriMesh();
	///Frees the memory allocated to vertices, normals etc..
	~TriMesh();

	/*! \brief The update function act just like constructors, except that they can be invoked anywhere in the program

	The update function is used to update the trimesh. The user can change the Trimesh loaded or move it to a different position and orientation.
	This function is useful in callbacks where the user wants to change the rendered model based on some input.*/
	void update(char* m_FileName);
	
	/*! \brief The update function act just like constructors, except that they can be invoked anywhere in the program

	The update function is used to update the trimesh. The user can change the Trimesh loaded or move it to a different position and orientation.
	This function is useful in callbacks where the user wants to change the rendered model based on some input.*/
	void update(char* m_FileName, HLdouble scale, HLdouble m_PositionX, HLdouble m_PositionY, HLdouble m_PositionZ);
    
	/*! \brief The update function act just like constructors, except that they can be invoked anywhere in the program

	The update function is used to update the trimesh. The user can change the Trimesh loaded or move it to a different position and orientation.
	This function is useful in callbacks where the user wants to change the rendered model based on some input.*/
	void update(char* m_FileName, HLdouble scale, HLdouble m_PositionX, HLdouble m_PositionY, HLdouble m_PositionZ, HLdouble AxisX, HLdouble AxisY, HLdouble AxisZ, HLdouble m_RotationFlag, bool m_FacetNormalsFlag);


	
	/*! \brief This function is used to search for a mesh by a name and return a pointer to the Trimesh if found.
	
	The name is assigned by the QHRenderer::setName function. If the function does not find a match it will return a NULL pointer.
	It is the responsibility of the user to detect the NULL return and handle it in a safe manner*/
	static TriMesh* searchTriMesh(char* TriMeshName);
	/*! \brief This function is used to search for a mesh by a ShapeID and return a pointer to the Trimesh if found.
	
	A shapeID is assigned to every haptic object by OpenHaptics. This is a unique number identifying
	the object under a fixed rendering context. This function can be used in events like "touch" or "untouch" etc...
	which can be setup to invoke the callback functions and pass the ShapeID.
	The Shape ID can then be used to return a pointer to the primitive shape.
	If the function does not find a match it will return a NULL pointer.
	It is the responsibility of the user to detect the NULL return and handle it in a safe manner*/
	static TriMesh* searchTriMesh(HLuint TriMeshID);
	
	
	/*! \brief This function can be called by the user to avoid using the display list and redraw the model every frame by parsing through the vertex data

	While attempting to display deformable models, it is necessary to make the mode dynamic.This is because QuickHaptics displays models by
	generating graphic display lists. A displaylist is by nature a static list of vertices. Therefore in order to make the vertices update
	at every frame the TriMesh has to declared dynamic*/
	void dynamic(bool m_DynamicMeshFlag);
	/*! \brief Function to search the binary tree to find the closest vertes to the given Position

	This function hard searches [O(n)] through the vertices of the model to find the vertex closest to the given position. Use this
	function with care for large models*/
	GLuint searchBinaryTree(hduVector3Dd Position);
	/*! \brief EXPERIMENTAL!!! Function to calculate the deformation of a model using Euler integration. 
	User should input the location of the cursor and the force exerted.

	This is an experimental section of the QuickHaptics API. We may plan to extend support for this feature in the future. Please refer to the SpongyCow
	Example for reference on how to use the deformation function*/
	void deformationFunction(hduVector3Dd CursorLocation, hduVector3Dd m_Force);
	/*! \brief EXPERIMENTAL!!! Function to calculate the deformation of a model using Euler integration. 
	This function calculated the deformation of the model when the user is no longer in contact with the model.

	This is an experimental section of the QuickHaptics API. We may plan to extend support for this feature in the future. Please refer to the SpongyCow
	Example for reference on how to use the deformation function. */
	void deformationFunction(void);
	/*! \brief This function returns the filename of the Mesh file parsed.

	This function returns the string containing the filename of the model*/
	char* getFileName(void);

	/*! \brief This function sets the mass per vertex for the spring mass system
	
	The deformable mesh is modeled as a point mass cloud connected with springs and dampers. This function sets the mass of each point in the mesh.*/
    void setMass(GLdouble m_Mass);
    
	/*! \brief This function sets the damping constant for the spring mass system
	
	The deformable mesh is modeled as a point mass cloud connected with springs and dampers. This function sets the damping of each damper*/
	void setSpringDamping(GLdouble m_SpringDamping);
    /*! \brief This function sets the spring constant for the spring mass system
	
	The deformable mesh is modeled as a point mass cloud connected with springs and dampers. This function sets the stiffness of each spring*/
	void setSpringStiffness(GLdouble m_SpringStiffness);
	/*! \brief This function either turns the gravity on or off
	
	This function either turns on the gravity or off in the environment (Gravity is always in the negative Y direction)*/
	void setGravity(bool GravityPresent);
	/*! \brief This function rotates the TriMesh Model by the angle specified, about it's centroid.*/
	void setRotationAboutCentroid(hduVector3Dd Axis, HLdouble Angle);
	/*! \brief This function returns the centroid of a model in model coordinates, The user SHOULD multiply the returned value with the 
	Model transform if the centroid in world coordinates is desired.*/
	hduVector3Dd getCentroid(void);

};

#endif