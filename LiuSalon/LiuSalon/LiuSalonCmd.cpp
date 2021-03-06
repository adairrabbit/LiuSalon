#include "LiuSalonCmd.h"
#include "cyHairFile.h"

#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include <list>
#include <sstream>

// command line args
const char *strandsFlag = "-s", *strandsLongFlag = "-strands";
const char *fileFlag = "-f", *fileLongFlag = "-filename";
const char *pointsFlag = "-p", *pointsLongFlag = "-points";
const char *lengthFlag = "-l", *lengthLongFlag = "-length";

LiuSalonCmd::LiuSalonCmd() : MPxCommand()
{
}

LiuSalonCmd::~LiuSalonCmd() 
{
}

MSyntax LiuSalonCmd::newSyntax()
{
    MSyntax syntax;

    syntax.addFlag(strandsFlag, strandsLongFlag, MSyntax::kLong);
	syntax.addFlag(fileFlag, fileLongFlag, MSyntax::kString);
	syntax.addFlag(pointsFlag, pointsLongFlag, MSyntax::kLong);
	syntax.addFlag(lengthFlag, lengthLongFlag, MSyntax::kDouble);

	return syntax;
}

MStatus LiuSalonCmd::doIt( const MArgList& args )
{
	MStatus stat = MS::kSuccess; 
	setResult( "Hair geometry has been created!\n" );
	MArgDatabase argData(syntax(), args);
	int hairCount = 0;
	MString filename = "";
	cyHairFile* h = new cyHairFile();

    if(argData.isFlagSet(fileFlag))
		argData.getFlagArgument(fileFlag, 0, filename);

	// filename: straight.hair, dark.hair, blonde.hair, natural.hair, wWavyThin.hair, wWavy.hair, wStraight.hair, wCurly.hair
	hairCount = h->LoadFromFile(filename.asChar());
	MGlobal::executeCommand(MString("print \"") + hairCount  + "\\n\"");

	int hasSegments = h->GetHeader().arrays & CY_HAIR_FILE_SEGMENTS_BIT;
	//int hasThickness = h->GetHeader().arrays & CY_HAIR_FILE_THICKNESS_BIT;

	int num_of_segments = h->GetHeader().d_segments;

    if(argData.isFlagSet(strandsFlag))
		argData.getFlagArgument(strandsFlag, 0, hairCount);

	int currPtIndex= 0;
	for(int i=0; i< hairCount; ++i){

		if(hasSegments)
			num_of_segments = h->GetSegmentsArray()[i];

		MString createCurve = MString("curve -d ") + num_of_segments;
		//MGlobal::executeCommand(MString("circle -radius ") +  h->GetHeader().d_thickness );
		for(int j=0; j<= num_of_segments; j++)
		{	
			//float scale = hasThickness? h->GetThicknessArray()[currPtNum+1] / h->GetThicknessArray()[currPtNum] : 1.0;

			createCurve  = createCurve + " -p " + h->GetPointsArray()[currPtIndex  ] 
									   + " "    + h->GetPointsArray()[currPtIndex+2] 
									   + " "    + h->GetPointsArray()[currPtIndex+1];	
			currPtIndex += 3;
		}
		createCurve = createCurve + ";";

		MGlobal::executeCommand( createCurve );

		/*MGlobal::executeCommand( MString("extrude -po 1 -et 2 -ucp 1 -fpt true -upn true -sc 1.0")
										+ " -rsp 1 \"nurbsCircle1\" \"curve1\";");
		MGlobal::executeCommand( MString("select -r nurbsCircle1; doDelete;") );
		MGlobal::executeCommand( MString("select -r curve1; doDelete;")       );*/
	}

	MString groupExtrudeSurfaces = MString("select -r ");
	for(int i=1; i <= hairCount; ++i){
		groupExtrudeSurfaces = groupExtrudeSurfaces + "curve" + i + " ";
	}

	MGlobal::executeCommand(groupExtrudeSurfaces +"; group;");

	return MStatus::kSuccess;
}