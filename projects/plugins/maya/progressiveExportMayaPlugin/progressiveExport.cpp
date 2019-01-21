// Range, Progressive Engine 2018
#define WIN32_LEAN_AND_MEAN

#include <sys/types.h>
#include <maya/MStatus.h>
#include <maya/MPxCommand.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnPlugin.h>
#include <maya/MFnMesh.h>
#include <maya/MFnSet.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItMeshEdge.h>
#include <maya/MFloatVector.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatArray.h>
#include <maya/MObjectArray.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MFnDagNode.h>
#include <maya/MItDag.h>
#include <maya/MDistance.h>
#include <maya/MIntArray.h>
#include <maya/MIOStream.h>
#include <maya/MBoundingBox.h>
#include <maya/MGlobal.h> 
#include <maya/MStreamUtils.h>
#include <maya/MPlugArray.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MFnRenderLayer.h>
#include <maya/MViewport2Renderer.h>
#include <maya/MImage.h>
#include <maya/M3dView.h>
#include <maya/MrenderView.h>
#include <maya/MDagModifier.h>
#include <maya/MFnTransform.h>
#include <maya/MFnCamera.h>
#include <maya/MFnDagNode.h>
#include <maya/MEulerRotation.h>

#include <fstream>
#include <time.h>
#include <sstream>
#include <iomanip>
#include <vector>
#include <set>
#include <algorithm>
#include <filesystem>
#include <iterator>
#include <map>
#include <experimental/vector>

#include <Windows.h>

#include "Vertex.h"

using namespace std;
namespace fs = experimental::filesystem;

#if defined (_WIN32)
#define strcasecmp stricmp
#endif

#define NO_SMOOTHING_GROUP      -1
#define INITIALIZE_SMOOTHING    -2
#define INVALID_ID              -1


//////////////////////////////////////////////////////////////
class ProgressiveObject : public MPxFileTranslator
{
	// �����
	ofstream inputFile; // �������� ����
	set<pair<Material, int>, MaterialComparator> bufMaterials; // ����� ������� � ����������
	map <Vertex, Normal, VertexComparator > arrayVertex; // ������ ������ ��� ���� ������, ����� ��������������� ������������ � ����
	int currentVertex; // ���������� ������
	int currentIndexTexture; // ������� ������ ��������
	vector< pair<MString, vector<std::map <Vertex, Normal, VertexComparator >::iterator> >> arrayIndeceds; // ������ ���� �������� + ��� ������
	string texturesPath; // ���� � ��������� ������

public:
	// static �����
	static const char* const optionScript; // ������ ����� ��� ��������
	static const char* const optionScriptDefaultValues; // ������ �������� �� ���������

	// ������������
	ProgressiveObject();
	static void* creator(); // ������ ��������� ������

	// �����������
    virtual ~ProgressiveObject() {};

	// ������
    MStatus reader ( const MFileObject& file,
                             const MString& optionsString,
                             FileAccessMode mode);

    MStatus writer ( const MFileObject& file,
                             const MString& optionsString,
                             FileAccessMode mode );
    bool haveReadMethod () const;
    bool haveWriteMethod () const;
    MString defaultExtension () const;
    MFileKind identifyFile ( const MFileObject& fileName,
                                   const char* buffer,
                                   short size) const;





	MFnDagNode get_object_by_name(const char* name);


private:
    MStatus OutputPolygons( MDagPath& ); // ���������� ���������
    MStatus exportSelection(); // ������� ���������� �������
    MStatus exportAll(); // ������� ���� �������
	bool isVisible(MFnDagNode & fnDag, MStatus& status); // ����� ��� ������� ������ �� �����
	MStatus writePolyFromBuffer(MFnMesh& fnMesh,
		MDagPath& mdagPath, 
		MObject& set,
		MObject& comp,
		vector<std::map <Vertex, Normal, VertexComparator >::iterator>& indeces); // ����� �������� � �����
	void setToLongUnitName(const MDistance::Unit&, MString&); // ������ ������� ��������� � �����
	bool writeInfoFromFile(); // ����� ��� ��������� ���������� � ����
	bool writeTexturesFromFile(); // ����� ��� �������� � �����
	bool writeLineFromFile(const char* line); // ������ ������ � �����
	bool writeVertexsFromFile(); // ������ ������ � �����
	bool writeMeshesFromFile(); // ������ ���� ����� � �����
	bool writeOptionsFromFile(const MString& mname, const MString& option); // ������ 
	void clear(); // ������� ��� ������ ����� ���������

};

//////////////////////////////////////////////////////////////

const char * const ProgressiveObject::optionScript("progressiveExportOptions");
const char * const ProgressiveObject::optionScriptDefaultValues("instansing=0;");

//////////////////////////////////////////////////////////////


ProgressiveObject::ProgressiveObject():currentVertex(0), currentIndexTexture(0), texturesPath("")
{
#if WIN32
	TCHAR buf[300];
	GetModuleFileName(NULL, buf, 300);
#endif
	string b(buf);
	string::size_type pos = b.find_last_of(R"(\)");
	if(pos != -1)
		b = b.substr(0, pos);
	ifstream f(b + R"(\progressive_config.pcon)");
	if (!f)
	{
		MStreamUtils::stdErrorStream() << "Error load Progressive config file!\n";
		return;
	}
	while (true)
	{
		getline(f, texturesPath, '\n');
		if (!f)
		{
			texturesPath = "";
			MStreamUtils::stdErrorStream() << "Error read Progressive config file!\n";
			return;
		}
		int index = texturesPath.find(";");
		if (index != -1)
			texturesPath = texturesPath.erase(index);
		if (texturesPath != "")
		{
			int pos(0);
			for (int i(texturesPath.length() - 1); i >= 0; i--)
			{
				if (texturesPath[i] == ' ' || texturesPath[i] == '\t')
					pos = i;
				else
					break;
			}
			texturesPath.erase(pos);
			break;
		}
	}
	if (texturesPath.find("$textures = ") == 0)
		texturesPath = texturesPath.substr(string("$textures = ").length());
	else
	{
		texturesPath = "";
		MStreamUtils::stdErrorStream() << "Error parse Progressive config file!\n";
		return;
	}
	MStreamUtils::stdErrorStream() << "Progressive config file load succesful.\n";
	f.close();
}

//////////////////////////////////////////////////////////////

void* ProgressiveObject::creator()
{
    return new ProgressiveObject();
}

//////////////////////////////////////////////////////////////
MString ProgressiveObject::defaultExtension() const
{
	return "pro_object";
}

//////////////////////////////////////////////////////////////

MStatus ProgressiveObject::reader ( const MFileObject& file,
                                const MString& options,
                                FileAccessMode mode)
{
    fprintf(stderr, "ObjTranslator::reader called in error\n");
    return MS::kFailure;
}

//////////////////////////////////////////////////////////////

MStatus ProgressiveObject::writer ( const MFileObject& file,
                                const MString& options,
                                FileAccessMode mode )
{
	MStreamUtils::stdErrorStream() << "Range Progressive Engine. Exporting model(s) from maya. \n";

	MStatus status = MStatus::kSuccess;  // ������ ���������� �������
    
	// ��� �����
    MString mname = file.fullName();

	if ((mode == MPxFileTranslator::kExportAccessMode) ||
		(mode == MPxFileTranslator::kSaveAccessMode)) // ���� �� ������� ����� �������������� ��( ��� ��������� ��? )
	{
		status = exportAll(); // TODO: �����������
		clear();
		return MS::kFailure;
	}
    else if( mode == MPxFileTranslator::kExportActiveAccessMode ) // ���� ������� ����� �������������� ����������
        status = exportSelection();

	// TODO: ���������, ��� ����� ��� �������������� ���������� ������(� maya 2000, � ������ 3500)
	// TODO: �������� ���� ��������(��������� ������� + ���� �����) - �������� ������� � xray
	// TODO: ��������� ��������: ����� ���������(��� ������� ���������� ������, ...

	if (status != MStatus::kSuccess)
		return status;

	// ������� ����
	inputFile.open(mname.asChar(), std::ios_base::binary);
	if (!inputFile) // ���� ���� ������ �������(����� ������� � �.�.)
	{
		MStreamUtils::stdErrorStream() << "Error: The file " << mname.asChar() << " could not be opened for writing." << endl;
		clear();
		return MS::kFailure;
	}

	if (!writeInfoFromFile()) // ����� ��������� ����������
	{
		MStreamUtils::stdErrorStream() << "Error write data from " << mname.asChar() << " file.\n";
		clear();
		return MStatus::kFailure;
	}

	if (!writeMeshesFromFile()) // ����� ��� ���� � ����
	{
		MStreamUtils::stdErrorStream() << "Error write data from " << mname.asChar() << " file.\n";
		clear();
		return MStatus::kFailure;
	}
	if (!writeVertexsFromFile()) // ���������� ��� ������� � ����
	{
		MStreamUtils::stdErrorStream() << "Error write data from " << mname.asChar() << " file.\n";
		clear();
		return MStatus::kFailure;
	}
	if (!writeTexturesFromFile()) // ���������� ��� �������� � ����
	{
		MStreamUtils::stdErrorStream() << "Error write data from " << mname.asChar() << " file.\n";
		clear();
		return MStatus::kFailure;
	}
	if (!writeOptionsFromFile(mname, options)) // ���������� ��������� � ����
	{
		clear();
		return MStatus::kFailure;
	}
	
	MStreamUtils::stdErrorStream()  << "Model " << mname.asChar() << " export successeful." << endl;
	clear(); // ������� ��� ������
	return MStatus::kSuccess;
}
//////////////////////////////////////////////////////////////

void ProgressiveObject::setToLongUnitName(const MDistance::Unit &unit, MString& unitName)
{
    switch( unit ) 
	{
	case MDistance::kInches:
		/// Inches
		unitName = "inches";
		break;
	case MDistance::kFeet:
		/// Feet
		unitName = "feet";
		break;
	case MDistance::kYards:
		/// Yards
		unitName = "yards";
		break;
	case MDistance::kMiles:
		/// Miles
		unitName = "miles";
		break;
	case MDistance::kMillimeters:
		/// Millimeters
		unitName = "millimeters";
		break;
	case MDistance::kCentimeters:
		/// Centimeters
		unitName = "centimeters";
		break;
	case MDistance::kKilometers:
		/// Kilometers
		unitName = "kilometers";
		break;
	case MDistance::kMeters:
		/// Meters
		unitName = "meters";
		break;
	default:
		break;
	}
}
//////////////////////////////////////////////////////////////

bool ProgressiveObject::haveReadMethod () const
{
    return false;
}
//////////////////////////////////////////////////////////////

bool ProgressiveObject::haveWriteMethod () const
{
    return true;
}

//////////////////////////////////////////////////////////////
MPxFileTranslator::MFileKind ProgressiveObject::identifyFile(
	const MFileObject& fileName,
	const char* buffer,
	short size) const
{
	const char * name = fileName.name().asChar();
	int   nameLength = strlen(name);

	if ((nameLength > 12) && !strcasecmp(name + nameLength - 4, ".pro_object"))
		return kCouldBeMyFileType;
	else
		return kNotMyFileType;
}

//////////////////////////////////////////////////////////////

MStatus initializePlugin(MObject obj) 
{
    MFnPlugin plugin(obj, "Range Prograssive Engine", "1.0", "Any"); // �������������� ������: ������, ��������� �� Maya, ��� ����������, ������, ������ API maya

	// ������������ ���������� ������ �����
    return plugin.registerFileTranslator( "Progressive object export",  // ��� ����� ������������ � ����� �������
										  "none", // ���� � ������
                                          ProgressiveObject::creator, // �������, ������� ������������ ����� ����
                                          ProgressiveObject::optionScript, // mel - ������, ������� ���������� ����� ��������� ���������� ����� � ��������� ���� ����������
										  ProgressiveObject::optionScriptDefaultValues); // �������� ���������� �� - ���������, ������� ����� �������� ����������� ����                      
}
//////////////////////////////////////////////////////////////

MStatus uninitializePlugin( MObject obj )
{
        MFnPlugin plugin( obj );
        return plugin.deregisterFileTranslator( "Progressive object export" );
}

//////////////////////////////////////////////////////////////

MStatus ProgressiveObject::OutputPolygons(MDagPath& mdagPath)
{
	// ������� ����� ��������� ������, ��������, ���������� ���������

	// ��������� ����������
	MStatus stat = MS::kSuccess; // ������ ���������� ������
	vector<map <Vertex, Normal, VertexComparator >::iterator> indeces; // ������ ��������
	int instanceNum(0);
	MObjectArray sets;
	MObjectArray comps;

	// �������� ������ ���������� ��� ����� ����
	if (mdagPath.isInstanced())
		instanceNum = mdagPath.instanceNumber();

	MFnMesh fnMesh(mdagPath, &stat);
	if (stat != MS::kSuccess)
	{
		MStreamUtils::stdErrorStream() << "Failure in MFnMesh initialization.\n";
		return MS::kFailure;
	}

	// ���������� ��� ������
	MString nameMesh = fnMesh.name();

	if (!fnMesh.getConnectedSetsAndMembers(instanceNum, sets, comps, true))
	{
		MStreamUtils::stdErrorStream() << "ERROR: MFnMesh::getConnectedSetsAndMembers\n";
		return MS::kFailure;
	}

	// ���������� ��� ���������, � ��������� ������ ���������, ������� ������������ ����������� ���������
	for (unsigned i = 0; i < sets.length(); i++)
	{
		MStatus stat = writePolyFromBuffer(fnMesh, mdagPath, sets[i], comps[i], indeces);
		if (stat == MStatus::kNotImplemented)
			continue;
		else if (stat == MStatus::kFailure)
			return stat;
	}

	// ����� ��� ���� + ������ �������� �� ��������� �����
	arrayIndeceds.push_back({ nameMesh , indeces });

	return MStatus::kSuccess;
}

//////////////////////////////////////////////////////////////

MStatus ProgressiveObject::exportSelection( )
{
	MStatus status;
	MString filename;

	MSelectionList slist;  // ������ ��������
	MGlobal::getActiveSelectionList( slist ); // �������� ������ ���������� �������� � maya
	MItSelectionList iter( slist ); // ������� �������� �� ������ ������� �� ���������� ��������

	if (iter.isDone())  // ���� ���������� �������� ���(���� ������ ����)
	{
		MStreamUtils::stdErrorStream() << "Error: Nothing is selected.\n"; // �������� �������� � �����, ��� �������� �� ��������
	    return MS::kFailure; // ���������� ������
	}

    // �������������� �������� ��������, ��� ������ ������ ��������
	// 1 - ������� ������� �������� �� ���������
	// 2 - ���������� ���������(����������� ��������)
	// 3 - ������ �������� ���������
    MItDag dagIterator( MItDag::kDepthFirst, MFn::kInvalid, &status);

	// � ����� �������� ��� ���������� ������
	for ( ; !iter.isDone(); iter.next() )
	{	 
		MDagPath objectPath; // ������ ��������� �� ������ ��������
		status = iter.getDagPath( objectPath); // �������� ������ ��������� �� �������� �������

		//  ���������� �������� �������� �� ��������� ����
		status = dagIterator.reset (objectPath.node(), 
									MItDag::kDepthFirst, MFn::kInvalid );	

		// ���������� ��� ���������� �� ������ �������
		for ( ; !dagIterator.isDone(); dagIterator.next() )
		{
			MDagPath dagPath; // ������ ��������� ���� �� �������(���������)
			status = dagIterator.getPath(dagPath); // �������� ������� ����

			if (!status) // ���� �� ����������
			{ 
				MStreamUtils::stdErrorStream() << "Failure getting DAG path.\n"; // ��������� �� ������ � �����
				return MS::kFailure;
			}

			if (status) // ������ ���������
			{
                MFnDagNode dagNode( dagPath, &status ); // ����� ������� � �����
                if (dagNode.isIntermediateObject())  // ���� ������ �������������(��� ������������ � �.�.)
                    continue; // �� ������������ ���

				// ���������� ������, ���� �������� ����� ������� �������������� �����������
				if (dagPath.hasFn(MFn::kNurbsSurface))  // ���� ��������� ������������ NURBS ���������
				{
					status = MS::kSuccess;
					MStreamUtils::stdErrorStream() << "Warning: skipping Nurbs Surface.\n"; // ����� ��������� � ���, ��� ����� �������� �� �������������� � ������������
				}
				else if ((  dagPath.hasFn(MFn::kMesh)) && (  dagPath.hasFn(MFn::kTransform))) // ���� ��� ��� � ������������� - �� ������� �� ��������� �����
					continue;
				else if (  dagPath.hasFn(MFn::kMesh)) // ���� ��� ������ ��� - �� ��� ������������
				{
					status = OutputPolygons(dagPath); // ���������� ���������
					if (status != MS::kSuccess) // ������ ������ ������ � ������
					{
						MStreamUtils::stdErrorStream() << "Error: exporting geom failed, check your selection.\n";
						return status;
					}
				}
			}
		}
	}
		
	return status;
}

//////////////////////////////////////////////////////////////
MFnDagNode ProgressiveObject::get_object_by_name(const char* name)
{
	MStatus status;
	MItDag dagIter(MItDag::kDepthFirst, MFn::kInvalid, &status);
	for (; !dagIter.isDone(); dagIter.next()) {
		MDagPath dagPath;
		status = dagIter.getPath(dagPath);

		MFnDagNode nodeFn;
		nodeFn.setObject(dagPath);
		if (strcmp(nodeFn.name().asChar(), name) == 0)
		{
			return dagPath;
		}
	}
	return MDagPath();
}


MStatus ProgressiveObject::exportAll( )
{
	MStatus status = MS::kSuccess;
	int count_model(0);
	vector<MObject> vecObj;

	MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid, &status);

	if ( MS::kSuccess != status) {
	    fprintf(stderr,"Failure in DAG iterator setup.\n");
	    return MS::kFailure;
	}

	for ( ; !dagIterator.isDone(); dagIterator.next() )
	{
		MDagPath dagPath;
		MObject  component = MObject::kNullObj;
		status = dagIterator.getPath(dagPath);

		if (!status) {
			fprintf(stderr,"Failure getting DAG path.\n");
			return MS::kFailure;
		}

		MFnDagNode dagNode( dagPath, &status );
		if (dagNode.isIntermediateObject()) 
		{
			continue;
		}

		if ((  dagPath.hasFn(MFn::kNurbsSurface)) &&
			(  dagPath.hasFn(MFn::kTransform)))
		{
			status = MS::kSuccess;
			fprintf(stderr,"Warning: skipping Nurbs Surface.\n");
		}
		else if ((  dagPath.hasFn(MFn::kMesh)) &&
				 (  dagPath.hasFn(MFn::kTransform)))
		{
			continue;
		}
		else if (  dagPath.hasFn(MFn::kMesh))
		{			
			MObject obj(dagPath.node());
			vecObj.push_back(obj);
			count_model++;
			
			/*status = OutputPolygons(dagPath, component);
			if (status != MS::kSuccess) {
				fprintf(stderr,"Error: exporting geom failed.\n");
                return MS::kFailure;
            }*/
		}
	}

	// �����!!!!!!!

	//MObjectArray arr(vecObj.size());
	//for (int i(0); i < arr.length(); i++)
	//	arr[i] = vecObj[i];

	//MFnRenderLayer* renderLayer = new MFnRenderLayer();

	//MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();
	//
	//// ������� ������� ����������� 512 x 512 � ��������� �� ����
	//MImage img;
	//unsigned char pixels[512 * 515 * 4];
	//for (int i = 0; i < 512 * 515 * 4; i += 4)
	//{
	//	pixels[i] = pixels[i + 3] = (char)255;
	//	pixels[i + 1] = pixels[i + 2] = (char)0;
	//}
	//img.setPixels(pixels, 512, 512);
	//img.writeToFile(R"(C:\Users\admin_test\Desktop\test.jpg)", "jpg");
	//

	//M3dView m3d = M3dView::active3dView(&status);
	//if (status != MStatus::kSuccess)
	//	return status;
	//MImage img;

	//// create camera - ok !
	//MDagModifier dagModifier;
	//MObject cameraTransformObj = dagModifier.createNode("transform");
	//dagModifier.renameNode(cameraTransformObj, "myCameraTransform");
	//MFnTransform transformFn(cameraTransformObj);
	//transformFn.setTranslation(MVector(0, 0, 40), MSpace::kTransform);

	//MObject cameraShapeObj = dagModifier.createNode("camera", cameraTransformObj);
	//dagModifier.renameNode(cameraShapeObj, "myCameraShape");
	//dagModifier.doIt();

	//MDagPath cam;
	//if (cameraShapeObj.hasFn(MFn::kDagNode))
	//{
	//	cam = MDagPath::getAPathTo(cameraShapeObj);
	//	m3d.setCamera(cam);
	//	m3d.refresh(); // ��� - �� ������� ���������� ��������
	//}
	//else
	//	return MStatus::kFailure;
	////






	// �������� ������ �����������
	//MDagPath dag = get_object_by_name("persp").dagPath();
	//MFnTransform trans(dag);
	//// �������� ������� � ���������� � ������ ���������
	//MVector pos = trans.getTranslation(MSpace::kTransform);
	//trans.setTranslation(MVector(-pos.x, -pos.y, -pos.z), MSpace::kTransform);
	//// �������� ������� � ������� �� 0
	//MEulerRotation rotate;
	//trans.getRotation(rotate);
	//MEulerRotation newRotate(rotate);
	//newRotate.x = -newRotate.x;
	//newRotate.y = -newRotate.y;
	//newRotate.z = -newRotate.z;
	// ���������� ������ �� 40 ������ ����� �� z
	//trans.setTranslation(MVector(0, 0, -40), MSpace::kTransform);



	//// ���������� ������ ������
	//MDagPath cameraOldPath;
	//m3d.getCamera(cameraOldPath);

	//m3d.setObjectDisplay(M3dView::DisplayObjects::kDisplayMeshes);
	//m3d.refresh(true, true);

	/*m3d.readColorBuffer(img, true);

	status = img.writeToFile(R"(C:\Users\admin_test\Desktop\test.jpg)", "jpg");
	if (status != MStatus::kSuccess)
		return status;*/

	return status;
}


bool ProgressiveObject::isVisible(MFnDagNode & fnDag, MStatus& status)
{
	/*if (fnDag.isIntermediateObject()) 
		return false;*/

	MPlug visPlug = fnDag.findPlug("visibility", &status); // ���������� ��������� ���������
	if (MStatus::kFailure == status)  // �� ������� ��������� ���������
	{
		MStreamUtils::stdErrorStream() << "MPlug::findPlug error!\n";
		//MGlobal::displayError("MPlug::findPlug");
		return false;
	}
	else 
	{
		bool visible;
		status = visPlug.getValue(visible); // �������� �������� ���������
		if (MStatus::kFailure == status)
		{
			//MGlobal::displayError("MPlug::getValue");
			MStreamUtils::stdErrorStream() << "MPlug::getValue error!\n";
			return false;
		}
		return visible;
	}
}


MStatus ProgressiveObject::writePolyFromBuffer(MFnMesh& fnMesh, 
	MDagPath& mdagPath, 
	MObject& set,
	MObject& comp,
	vector<std::map <Vertex, Normal, VertexComparator >::iterator>& indeces)
{
	// ����������
	MStatus stat = MStatus::kSuccess; // ������ ���������� ������
	MSpace::Space space = MSpace::kWorld; // ����������(�������)


	MFnSet fnSet(set, &stat); // ������� �����
	if (stat == MS::kFailure)   // �� ������� �������
	{
		MStreamUtils::stdErrorStream() << "ERROR: MFnSet::MFnSet\n";
		return MStatus::kNotImplemented;
	}

	MItMeshPolygon polyIter(mdagPath, comp, &stat); // �������� �� �������
	if ((stat == MS::kFailure)/* || comp.isNull()*/) // �� ������� ������� ��������
	{
		MStreamUtils::stdErrorStream() << "ERROR: Can't create poly iterator!\n";
		return MStatus::kNotImplemented;
	}

	MFnDependencyNode fnNode(set); // �������� ��������
	MPlug shaderPlug = fnNode.findPlug("surfaceShader");
	MPlugArray connectedPlugs;
	if (shaderPlug.isNull())
	{
		MStreamUtils::stdErrorStream() << "ERROR: Can't find material!\n";
		return MStatus::kNotImplemented;
	}
	shaderPlug.connectedTo(connectedPlugs, true, false);

	MFnDependencyNode fnDN(connectedPlugs[0].node()); // �������� ��� ���������

	MItDependencyGraph dgIt(shaderPlug, MFn::kFileTexture, // �������� ���� �������
		MItDependencyGraph::kUpstream,
		MItDependencyGraph::kBreadthFirst,
		MItDependencyGraph::kNodeLevel,
		&stat);
	if (stat == MS::kFailure) //�� ������ �������� ���� �������
	{
		MStreamUtils::stdErrorStream() << "ERROR: Can't load graph textures\n";
		return MStatus::kNotImplemented;
	}
	dgIt.disablePruningOnFilter();
	if (dgIt.isDone()) // ������� ���
	{
		MStreamUtils::stdErrorStream() << "Warning: Material " << fnDN.name().asChar() << " not textured\n";
		//return MStatus::kNotImplemented;
	}

	MObject textureNode = dgIt.thisNode(); // �������� ��� �������
	MPlug filenamePlug = MFnDependencyNode(textureNode).findPlug("fileTextureName");
	MString textureName; // ��� ��������
	filenamePlug.getValue(textureName);
	Material mat; // ������ ��������
	mat.nameMaterial = fnDN.name().asChar(); // ���������� ��� ���������
	mat.nameTexture = textureName.asChar(); // ���������� ��� ��������
	int index(-1);
	if (texturesPath != "") // �������� ���� � ��������
	{
		if (mat.nameTexture.find(texturesPath) == 0)
		{
			index = mat.nameTexture.find("engine_resource");
			mat.nameTexture = mat.nameTexture.substr(index);
		}
		else
		{
			index = mat.nameTexture.find_last_of(R"(/)");
			if (index != -1)
				mat.nameTexture = mat.nameTexture.substr(index + 1);
		}
	}
	currentIndexTexture = ((bufMaterials.insert({ mat, bufMaterials.size() })).first)->second; // �������� �������� � �����

	// ����� ������������ ��� ��������, ������� ����������� ����� ��������� � ��� �� ������ ������������ � ���� �����(����� ������� ����� � ����)
	for (; !polyIter.isDone(); polyIter.next())
	{
		int polyVertexCount = polyIter.polygonVertexCount(); // �������� ���������� ������ � ��������
		if (polyVertexCount != 3)  // ��������, ��������������� �� ������
		{
			MStreamUtils::stdErrorStream() << "Model " << fnMesh.name().asChar() << " is not triangulate!\n";
			return MS::kFailure;
		}
		for (int vtx = 0; vtx < polyVertexCount; vtx++) // ���������� ��� ������� � ��������
		{
			// ����������
			Vertex vertex;

			// �������� �������
			MPointArray vert;
			polyIter.getPoints(vert, space);
			vertex.pos = { (float)vert[vtx].x, (float)vert[vtx].y, (float)-vert[vtx].z }; // z - ���������� �������������

			// �������� ���������� ���������� �������
			MFloatArray us;
			MFloatArray vs;
			polyIter.getUVs(us, vs);
			vertex.uv = { us[vtx] , 0 - vs[vtx] , (float)currentIndexTexture }; // z - ������ �������� �� ������ ������� �������

			// �������� �������
			MVectorArray norm;
			polyIter.getNormals(norm, space);
			vertex.normal = { (float)norm[vtx].x, (float)norm[vtx].y, (float)norm[vtx].z };

			// ������������ ������ ������
			auto iter = arrayVertex.find(vertex);
			if (iter != arrayVertex.end()) // ���� ����� ������� ��� ����
			{
				iter->second.normal += vertex.normal;
				iter->second.count++;
				indeces.push_back(iter); // �� ��������� � ������� �������� ����� ������
			}
			else // ����� ��������� ����� ������� � � ������
			{
				iter = arrayVertex.insert({ vertex, Normal(vertex.normal, 1) }).first; // �������� ������� 
				indeces.push_back(iter); // �������� ������
				currentVertex++; // ����������� ������� �������
			}
		}
		std::swap(indeces.back(), indeces[indeces.size() - 3]); // �������������� �����������(����� � ������ ��� ����� � ������)
	}
	return MStatus::kSuccess;
}

//////////////////////////////////////////////////////////////
bool ProgressiveObject::writeInfoFromFile()
{
	// ����� � ����� ����� ����� - ���� ��������� ����������
	int block = 0x10;
	inputFile.write((char*)&block, 4 );
	if (!inputFile)
		return false;
	// ����� � ����� ������ � ������������, ������� ������ ��� ������
	char* name = getenv("USERNAME");
	writeLineFromFile(name);
	// ����� � ����� ������ � ���� � ������� �������� ������
	tm *tim;
	time_t tt = time(NULL);
	tim = localtime(&tt);
	DateTime dt(tim->tm_sec, tim->tm_min, tim->tm_hour, tim->tm_mday, tim->tm_mon, tim->tm_year);
	inputFile.write((char*)&dt, sizeof(DateTime));
	if (!inputFile)
		return false;
	// ����� � ���� ������ � ���� � ������� ����������� ������(������������ � ���������)
	inputFile.write((char*)&dt, sizeof(DateTime));
	if (!inputFile)
		return false;
	// ����� � ���� ���������� ����� � ������
	MSelectionList slist;  // ������ ��������
	MGlobal::getActiveSelectionList(slist); // ��������� ������ �������� �����������(���� export all - �� �������� ���)
	int length(slist.length());
	inputFile.write((char*)&length, 4);
	if (!inputFile)
		return false;
	// ����� � ����� ����� ������(�����)
	//vector<MPoint> centers;
	vector< MBoundingBox> bboxArr;
	for (MItSelectionList iter(slist); !iter.isDone(); iter.next())
	{
		MDagPath path;
		iter.getDagPath(path);
		MFnMesh fnMesh(path); // ������ � ����
		MBoundingBox bbox = fnMesh.boundingBox(); // �������������� ����
		//centers.push_back(bbox.center()); // ���������� �����
		bboxArr.push_back(bbox); // ���������� bbox
	}
	// ������������ ����� �����
	MPoint center; // ������������ �����
	float xl(bboxArr[0].min().x), xr(bboxArr[0].max().x), yd(bboxArr[0].min().y), yu(bboxArr[0].max().y), zr(-bboxArr[0].min().z), zf(-bboxArr[0].max().z); // ������� �������. ������
	if (bboxArr.size() == 1)
	{
		center = bboxArr[0].center();
		center.z = -center.z;
	}
	else
	{
		for (auto&bb : bboxArr)
		{
			auto p = bb.min(); p.z = -p.z;
			auto p1 = bb.max(); p1.z = -p1.z;
			if (xl > p.x)
				xl = p.x;
			if (xr < p1.x)
				xr = p1.x;
			if (yd > p.y)
				yd = p.y;
			if (yu < p1.y)
				yu = p1.y;
			if (zr > p.z)
				zr = p.z;
			if (zf < p1.z)
				zf = p1.z;
		}
		center = { (xr - xl) / 2, (yu - yd) / 2, (zf - zr) / 2 };
	}
	float cx = center.x;
	inputFile.write((char*)&cx, sizeof(float));
	if (!inputFile)
		return false;
	float cy = center.y;
	inputFile.write((char*)&cy, sizeof(float));
	if (!inputFile)
		return false;
	float cz = center.z;
	inputFile.write((char*)&cz, sizeof(float));
	if (!inputFile)
		return false;
	// ����� � ����� ������� ���������� BBox(������ ��� �������� ������)
	//xl--; yd--; zf--; xr++; yu++; zr++;
	inputFile.write((char*)&xl, sizeof(float));
	if (!inputFile)
		return false;
	inputFile.write((char*)&yd, sizeof(float));
	if (!inputFile)
		return false;
	inputFile.write((char*)&zf, sizeof(float));
	if (!inputFile)
		return false;
	inputFile.write((char*)&xr, sizeof(float));
	if (!inputFile)
		return false;
	inputFile.write((char*)&yu, sizeof(float));
	if (!inputFile)
		return false;
	inputFile.write((char*)&zr, sizeof(float));
	if (!inputFile)
		return false;
	// ���������� ������� ��������� � ������ ����
	MString unitName;
	setToLongUnitName(MDistance::uiUnit(), unitName);
	if (!writeLineFromFile(unitName.asChar()))
		return false;

	return true;
}

//////////////////////////////////////////////////////////////
bool ProgressiveObject::writeTexturesFromFile()
{
	int block = 0x40;
	inputFile.write((char*)&block, 4 ); // ����� ������� ����(�������)
	if (!inputFile)
		return false;
	int s = bufMaterials.size();
	inputFile.write((char*)&s, 4 ); // ����� ���-�� �������
	if (!inputFile)
		return false;
	for (auto& t : bufMaterials)
	{
		inputFile.write((char*)&t.second, 4 ); // ����� ������ ��������
		if (!inputFile)
			return false;
		if (!writeLineFromFile(t.first.nameTexture.c_str())) // ����� ���� � ��������
			return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////
bool ProgressiveObject::writeLineFromFile(const char* line)
{
	for (int i(0); i < strlen(line); i++)
	{
		inputFile.write((char*)&line[i], 1); // ����� �� �������
		if (!inputFile)
			return false;
	}
	char null('\0');
	inputFile.write((char*)&null, 1 ); // ����� ���� - ��������
	if (!inputFile)
		return false;
	return true;
}

//////////////////////////////////////////////////////////////

bool ProgressiveObject::writeVertexsFromFile()
{
	int block = 0x30;
	inputFile.write((char*)&block, 4 ); // ����� ������� ����(�������)
	if (!inputFile)
		return false;
	int s = arrayVertex.size();
	inputFile.write((char*)&s, 4 ); // ���������� ������
	if (!inputFile)
		return false;
	for (auto& v : arrayVertex)
	{
		auto ver = v.first; // �������� �������
		ver.normal = v.second.normal * (1 /  v.second.count); // ������������ �� �������
		inputFile.write((char*)&v, sizeof(Vertex)); // ����� ������
		if (!inputFile)
			return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////

bool ProgressiveObject::writeMeshesFromFile()
{
	int block = 0x20;
	inputFile.write((char*)&block, 4 ); // ����� ������� ����(�������)
	if (!inputFile)
		return false;
	for (auto& ind : arrayIndeceds)
	{
		// ����� id ��������
		block += 0x0001; // ����������� ����� �����
		inputFile.write((char*)&block, 4 ); // ����� ������� ����
		if (!inputFile)
			return false;
		//����� � ����� �� �������: ��� ����, ���-�� ��������, ����� ��������
		if (!writeLineFromFile(ind.first.asChar())) // ��� ����
			return false;
		int s = ind.second.size();
		inputFile.write((char*)&s, 4 ); // ���������� ��������
		if (!inputFile)
			return false;
		for (auto& i : ind.second)
		{
			int index = std::distance(arrayVertex.begin(), i);
			inputFile.write((char*)&index, 4 ); // ����� ��������
			if (!inputFile)
				return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////

bool ProgressiveObject::writeOptionsFromFile(const MString& mname, const MString & option)
{
	// ���������
	bool isInstansing(false);
	//

	int block = 0x50;
	inputFile.write((char*)&block, 4); // ����� ������� ����(�������)
	if (!inputFile)
	{
		MStreamUtils::stdErrorStream() << "Error write data from " << mname.asChar() << " file.\n";
		return false;
	}
	if (option.length() > 0) // ��������� ����
	{
		MStringArray opt;
		option.split(L';', opt); 
		for(int i(0);i<opt.length();i++)
		{
			MStringArray currentOpt;
			opt[i].split(L'=', currentOpt);
			if (currentOpt[0] == "instansing")
				isInstansing = currentOpt[1].asInt();
		}
	}
	else
	{
		MStreamUtils::stdErrorStream() << "Error read option for export!\n";
		return false;
	}
	// ����� � ����
	inputFile.write((char*)&isInstansing, sizeof(bool)); // ������� ��� ���
	if (!inputFile)
	{
		MStreamUtils::stdErrorStream() << "Error write data from " << mname.asChar() << " file.\n";
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////
void ProgressiveObject::clear()
{
	bufMaterials.clear(); // ����� ������� � ����������
	arrayVertex.clear(); // ������ ������ ��� ���� ������, ����� ��������������� ������������ � ����
	currentVertex = 0; // ���������� ������
	currentIndexTexture = 0; // ������� ������ ��������
	arrayIndeceds.clear(); // ����� ���� �����
	inputFile.close(); // ��������� ����
}

//////////////////////////////////////////////////////////////
