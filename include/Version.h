#ifndef VERSION_H_
#define VERSION_H_

#include <string>
namespace mlib{

using namespace std;
#define GIT_SHA1 "51572ab-dirty" 
#define GIT_DATE "Wed Feb 8 19:41:20 2017" 
#define GIT_COMMIT_SUBJECT "add version.h" 

	static const string mlibVersionSha1= GIT_SHA1;
	static const string mlibVersionDate= GIT_DATE;
	static const string mlibVersionCommitSubject= GIT_COMMIT_SUBJECT;



}

#endif
