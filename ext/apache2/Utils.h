/*
 *  Phusion Passenger - http://www.modrails.com/
 *  Copyright (C) 2008  Phusion
 *
 *  Phusion Passenger is a trademark of Hongli Lai & Ninh Bui.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef _PASSENGER_UTILS_H_
#define _PASSENGER_UTILS_H_

#include <boost/shared_ptr.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <cstdio>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include "Exceptions.h"

typedef struct CachedMultiFileStat CachedMultiFileStat;

namespace Passenger {

using namespace std;
using namespace boost;

/** Enumeration which indicates what kind of file a file is. */
typedef enum {
	/** The file doesn't exist. */
	FT_NONEXISTANT,
	/** A regular file or a symlink to a regular file. */
	FT_REGULAR,
	/** A directory. */
	FT_DIRECTORY,
	/** Something else, e.g. a pipe or a socket. */
	FT_OTHER
} FileType;

/**
 * Convenience shortcut for creating a <tt>shared_ptr</tt>.
 * Instead of:
 * @code
 *    shared_ptr<Foo> foo;
 *    ...
 *    foo = shared_ptr<Foo>(new Foo());
 * @endcode
 * one can write:
 * @code
 *    shared_ptr<Foo> foo;
 *    ...
 *    foo = ptr(new Foo());
 * @endcode
 *
 * @param pointer The item to put in the shared_ptr object.
 * @ingroup Support
 */
template<typename T> shared_ptr<T>
ptr(T *pointer) {
	return shared_ptr<T>(pointer);
}

/**
 * Used internally by toString(). Do not use directly.
 *
 * @internal
 */
template<typename T>
struct AnythingToString {
	string operator()(T something) {
		stringstream s;
		s << something;
		return s.str();
	}
};

/**
 * Used internally by toString(). Do not use directly.
 *
 * @internal
 */
template<>
struct AnythingToString< vector<string> > {
	string operator()(const vector<string> &v) {
		string result("[");
		vector<string>::const_iterator it;
		unsigned int i;
		for (it = v.begin(), i = 0; it != v.end(); it++, i++) {
			result.append("'");
			result.append(*it);
			if (i == v.size() - 1) {
				result.append("'");
			} else {
				result.append("', ");
			}
		}
		result.append("]");
		return result;
	}
};

/**
 * Convert anything to a string.
 *
 * @param something The thing to convert.
 * @ingroup Support
 */
template<typename T> string
toString(T something) {
	return AnythingToString<T>()(something);
}

/**
 * Converts the given string to an integer.
 * @ingroup Support
 */
int atoi(const string &s);

/**
 * Converts the given string to a long integer.
 * @ingroup Support
 */
long atol(const string &s);

/**
 * Split the given string using the given separator.
 *
 * @param str The string to split.
 * @param sep The separator to use.
 * @param output The vector to write the output to.
 * @ingroup Support
 */
void split(const string &str, char sep, vector<string> &output);

/**
 * Check whether the specified file exists.
 *
 * @param filename The filename to check.
 * @param mstat A CachedMultiFileStat object, if you want to use cached statting.
 * @param throttleRate A throttle rate for mstat. Only applicable if mstat is not NULL.
 * @return Whether the file exists.
 * @throws FileSystemException Unable to check because of a filesystem error.
 * @ingroup Support
 */
bool fileExists(const char *filename, CachedMultiFileStat *mstat = 0,
                unsigned int throttleRate = 0);

/**
 * Check whether 'filename' exists and what kind of file it is.
 *
 * @param filename The filename to check.
 * @param mstat A CachedMultiFileStat object, if you want to use cached statting.
 * @param throttleRate A throttle rate for mstat. Only applicable if mstat is not NULL.
 * @return The file type.
 * @throws FileSystemException Unable to check because of a filesystem error.
 * @ingroup Support
 */
FileType getFileType(const char *filename, CachedMultiFileStat *mstat = 0,
                     unsigned int throttleRate = 0);

/**
 * Find the location of the Passenger spawn server script.
 * If passengerRoot is given, t T
 *
 * @param passengerRoot The Passenger root folder. If NULL is given, then
 *      the spawn server is found by scanning $PATH. For security reasons,
 *      only absolute paths are scanned.
 * @return An absolute path to the spawn server script, or
 *         an empty string on error.
 * @throws FileSystemException Unable to access parts of the filesystem.
 * @ingroup Support
 */
string findSpawnServer(const char *passengerRoot = NULL);

/**
 * Find the location of the Passenger ApplicationPool server
 * executable.
 *
 * @param passengerRoot The Passenger root folder.
 * @return An absolute path to the executable.
 * @throws FileSystemException Unable to access parts of the filesystem.
 * @pre passengerRoot != NULL
 * @ingroup Support
 */
string findApplicationPoolServer(const char *passengerRoot);

/**
 * Returns a canonical version of the specified path. All symbolic links
 * and relative path elements are resolved.
 *
 * @throws FileSystemException Something went wrong.
 * @ingroup Support
 */
string canonicalizePath(const string &path);

/**
 * If <em>path</em> refers to a symlink, then this function resolves the
 * symlink for 1 level. That is, if the symlink points to another symlink,
 * then the other symlink will not be resolved. The resolved path is returned.
 *
 * If the symlink doesn't point to an absolute path, then this function will
 * prepend <em>path</em>'s directory to the result.
 *
 * If <em>path</em> doesn't refer to a symlink then this method will return
 * <em>path</em>.
 *
 * @throws FileSystemException Something went wrong.
 * @ingroup Support
 */
string resolveSymlink(const string &path);

/**
 * Given a path, extracts its directory name.
 *
 * @ingroup Support
 */
string extractDirName(const string &path);

/**
 * Escape the given raw string into an XML value.
 *
 * @throws std::bad_alloc Something went wrong.
 * @ingroup Support
 */
string escapeForXml(const string &input);

/**
 * Given a username that's supposed to be the "lowest user" in the user switching mechanism,
 * checks whether this username exists. If so, this users's UID and GID will be stored into
 * the arguments of the same names. If not, <em>uid</em> and <em>gid</em> will be set to
 * the UID and GID of the "nobody" user. If that user doesn't exist either, then <em>uid</em>
 * and <em>gid</em> will be set to -1.
 */
void determineLowestUserAndGroup(const string &user, uid_t &uid, gid_t &gid);

/**
 * Return the path name for the directory in which the system stores general
 * temporary files. This is usually "/tmp", but might be something else depending
 * on some environment variables.
 *
 * @ensure result != NULL
 * @ingroup Support
 */
const char *getSystemTempDir();

/**
 * Return the path name for the directory in which Phusion Passenger-specific
 * temporary files are to be stored. This directory is unique for this instance
 * of the web server in which Phusion Passenger is running.
 *
 * If the environment variable PASSENGER_INSTANCE_TEMP_DIR is set, then that value
 * will be returned. If this environment variable is not set, then it will be set
 * with the return value.
 *
 * To bypass the usage of the PASSENGER_INSTANCE_TEMP_DIR environment variable,
 * set 'bypassCache' to true.
 *
 * @param bypassCache Whether PASSENGER_INSTANCE_TEMP_DIR should be bypassed.
 * @param systemTempDir The directory under which the Phusion Passenger-specific
 *                      temp directory should be located. If set to the empty string,
 *                      then the return value of getSystemTempDir() will be used.
 * @ensure !result.empty()
 * @ingroup Support
 */
string getPassengerTempDir(bool bypassCache = false, const string &systemTempDir = "");

/* Create a temp directory under <em>systemTempDir</em>, for storing Phusion
 * Passenger-specific temp files, such as temporarily buffered uploads,
 * sockets for backend processes, etc. This call also sets the
 * PASSENGER_INSTANCE_TEMP_DIR environment variable, which allows subprocesses
 * to find this temp directory.
 *
 * The created temp directory will have several subdirectories:
 * - webserver_private - for storing the web server's buffered uploads.
 * - info - for storing files that allow external tools to query information
 *          about a running Phusion Passenger instance.
 * - backends - for storing Unix sockets created by backend processes.
 * - var - for all other kinds of temp files.
 *
 * If a (sub)directory already exists, then it will not result in an error.
 *
 * The <em>userSwitching</em> and <em>lowestUser</em> arguments passed to
 * this method are used for determining the optimal permissions for the
 * (sub)directories. The permissions will be set as tightly as possible based
 * on the values. The <em>workerUid</em> and <em>workerGid</em> arguments
 * will be used for determining the owner of certain subdirectories.
 *
 * @note You should only call this method inside the web server's master
 *       process. In case of Apache, this is the Apache control process,
 *       the one that tends to run as root. This is because this function
 *       will set directory permissions and owners/groups, which may require
 *       root privileges.
 *
 * @param systemTempDir The directory under which the Phusion Passenger-specific
 *                      temp directory should be created. You should normally
 *                      specify the return value of getSystemTempDir().
 * @param userSwitching Whether user switching is turned on.
 * @param lowestUser The user that the spawn manager and the pool server will
 *                   run as, if user switching is turned off.
 * @param workerUid The UID that the web server's worker processes are running
 *                  as. On Apache, this is the UID that's associated with the
 *                  'User' directive.
 * @param workerGid The GID that the web server's worker processes are running
 *                  as. On Apache, this is the GID that's associated with the
 *                  'Group' directive.
 * @throws IOException Something went wrong.
 * @throws SystemException Something went wrong.
 * @throws FileSystemException Something went wrong.
 */
void createPassengerTempDir(const string &systemTempDir, bool userSwitching,
                            const string &lowestUser,
                            uid_t workerUid, gid_t workerGid);

/**
 * Create the directory at the given path, creating intermediate directories
 * if necessary. The created directories' permissions are as specified by the
 * 'mode' parameter. You can specify this directory's owner and group through
 * the 'owner' and 'group' parameters. A value of -1 for 'owner' or 'group'
 * means that the owner/group should not be changed.
 *
 * If 'path' already exists, then nothing will happen.
 *
 * @throws IOException Something went wrong.
 * @throws SystemException Something went wrong.
 * @throws FileSystemException Something went wrong.
 */
void makeDirTree(const string &path, const char *mode = "u=rwx,g=,o=", uid_t owner = -1, gid_t group = -1);

/**
 * Remove an entire directory tree recursively.
 *
 * @throws FileSystemException Something went wrong.
 */
void removeDirTree(const string &path);

/**
 * Check whether the specified directory is a valid Ruby on Rails
 * application root directory.
 *
 * @param mstat A CachedMultiFileStat object, if you want to use cached statting.
 * @param throttleRate A throttle rate for mstat. Only applicable if mstat is not NULL.
 * @throws FileSystemException Unable to check because of a system error.
 * @ingroup Support
 */
bool verifyRailsDir(const string &dir, CachedMultiFileStat *mstat = 0,
                    unsigned int throttleRate = 0);

/**
 * Check whether the specified directory is a valid Rack application
 * root directory.
 *
 * @param mstat A CachedMultiFileStat object, if you want to use cached statting.
 * @param throttleRate A throttle rate for mstat. Only applicable if mstat is not NULL.
 * @throws FileSystemException Unable to check because of a filesystem error.
 * @ingroup Support
 */
bool verifyRackDir(const string &dir, CachedMultiFileStat *mstat = 0,
                   unsigned int throttleRate = 0);

/**
 * Check whether the specified directory is a valid WSGI application
 * root directory.
 *
 * @param mstat A CachedMultiFileStat object, if you want to use cached statting.
 * @param throttleRate A throttle rate for mstat. Only applicable if mstat is not NULL.
 * @throws FileSystemException Unable to check because of a filesystem error.
 * @ingroup Support
 */
bool verifyWSGIDir(const string &dir, CachedMultiFileStat *mstat = 0,
                   unsigned int throttleRate = 0);

/**
 * Represents a buffered upload file.
 *
 * @ingroup Support
 */
class BufferedUpload {
public:
	/** The file handle. */
	FILE *handle;
	
	/**
	 * Create an empty upload bufer file, and open it for reading and writing.
	 *
	 * @throws SystemException Something went wrong.
	 */
	BufferedUpload(const char *identifier = "temp") {
		char templ[PATH_MAX];
		int fd;
		
		snprintf(templ, sizeof(templ), "%s/%s.XXXXXX", getDir().c_str(), identifier);
		templ[sizeof(templ) - 1] = '\0';
		fd = mkstemp(templ);
		if (fd == -1) {
			char message[1024];
			int e = errno;
			
			snprintf(message, sizeof(message), "Cannot create a temporary file '%s'", templ);
			message[sizeof(message) - 1] = '\0';
			throw SystemException(message, e);
		}
		
		/* We use a POSIX trick here: the file's permissions are set to "u=,g=,o="
		 * and the file is deleted immediately from the filesystem, while we
		 * keep its file handle open. The result is that no other processes
		 * will be able to access this file's contents anymore, except us.
		 * We now have an anonymous disk-backed buffer.
		 */
		fchmod(fd, 0000);
		unlink(templ);
		
		handle = fdopen(fd, "w+");
	}
	
	~BufferedUpload() {
		fclose(handle);
	}
	
	/**
	 * Returns the directory in which upload buffer files are stored.
	 * This is a subdirectory of the directory returned by getPassengerTempDir(). 
	 */
	static string getDir() {
		return getPassengerTempDir() + "/webserver_private";
	}
};

} // namespace Passenger

#endif /* _PASSENGER_UTILS_H_ */

