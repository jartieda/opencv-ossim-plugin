/*!
 *
 * OVERVIEW: Application to test ossimOpenCVGoodFeaturesToTrack 
 *
 */

#include <iostream>
#include <iterator>

#include <ossim/base/ossimFilename.h>
#include <ossim/base/ossimString.h>
#include <ossim/base/ossimStdOutProgress.h>

#include <ossim/imaging/ossimImageHandlerRegistry.h>
#include <ossim/imaging/ossimImageHandler.h>
#include <ossim/imaging/ossimImageFileWriter.h>
#include <ossim/imaging/ossimImageWriterFactoryRegistry.h>
#include <ossim/imaging/ossimImageSourceFactoryRegistry.h>
#include <ossim/imaging/ossimTiffWriter.h>

#include "../plugin/ossimOpenCVGoodFeaturesToTrack.h"

#include <ossim/init/ossimInit.h>


using namespace std;
void usage();
void printOutputTypes();

int main(int argc, char* argv[])
{

if(argc<3) {
	usage();
	return 0;
}

ossimKeywordlist kwl; 

ossimInit::instance()->initialize(argc, argv);

ossimImageHandler *handler = ossimImageHandlerRegistry::instance()->open(ossimFilename(argv[2]));

if(!handler)
       {
         cout << "Unable to open input image: "<< argv[2] << endl;
         return 1;
       }

ossimOpenCVGoodFeaturesToTrack * filter = new ossimOpenCVGoodFeaturesToTrack();  
if(kwl.addFile(argv[4])){
    filter->loadState(kwl);
}
filter->connectMyInputTo(0,handler);

ossimImageFileWriter* writer = ossimImageWriterFactoryRegistry::instance()->createWriter(ossimString(argv[1]));
writer->setFilename(ossimFilename(argv[3]));
writer->connectMyInputTo(filter);        

ossimStdOutProgress progress(0, true);
writer->addListener(&progress);
writer->execute();
writer->removeListener(&progress);

printf("Finished!\n");

std::vector<ossimDpt> features = filter->theKeyPoints;
printf("Number of key points: %d\n",features.size());
for (int i=0; i<features.size(); i++) {
	ossimDpt c = features.at(i);
	printf("(%f,%f)\n",c.x,c.y);
}

return 0;

}


void usage()   
{
  cout << "test_corners <output_type> <input_im> <output_im> [<spec_file>]" << endl 
       << "where output types are: " << endl;
  printOutputTypes();
}

void printOutputTypes()
{
   std::vector<ossimString> outputType;

   ossimImageWriterFactoryRegistry::instance()->getImageTypeList(outputType);
   std::copy(outputType.begin(),
             outputType.end(),
             std::ostream_iterator<ossimString>(cout, "\n"));
}
