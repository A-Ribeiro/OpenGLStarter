#ifndef platform_path_h
#define platform_path_h

#include <aribeiro/common.h>
#include <string>

namespace aRibeiro {

    /// \brief Common path operations
    ///
    /// \author Alessandro Ribeiro
    ///
    class PlatformPath {

    public:

        static std::string SEPARATOR;

        /// \brief Compute the executable path from the application first argument.
        ///
        /// Can be used in the main function.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// int main(int argc, char *argv[]) {
        ///     PlatformPath::setWorkingPath( PlatformPath::getExecutablePath( argv[0] ) );
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param argv0 argument 0 from the application main procedure
        /// \return the computed path
        ///
        static std::string getExecutablePath(const char* argv0);

        /// \brief Compute the executable path from the application first argument.
        ///
        /// Can be used in the main function.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// int main(int argc, char *argv[]) {
        ///     PlatformPath::setWorkingPath( PlatformPath::getExecutablePath( std::string( argv[0] ) ) );
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param arg0 argument 0 from the application main procedure
        /// \return the computed path
        ///
        static std::string getExecutablePath(const std::string &arg0);

        /// \brief Get the current executable path.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// std::string current_work_directory = PlatformPath::getWorkingPath();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return the current path
        ///
        static std::string getWorkingPath();

        /// \brief Set the current executable path.
        ///
        /// It helps to set the current relative path, that all commands related to FILE manipulation work with.
        ///
        /// Can be used in the main function.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// int main(int argc, char *argv[]) {
        ///     PlatformPath::setWorkingPath( PlatformPath::getExecutablePath( argv[0] ) );
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param path the new path to set
        /// \return true if its OK
        ///
        static bool setWorkingPath(const std::string &path);

        /// \brief Get and create the savegame path based to a root folder and game name.
        ///
        /// In windows, the code gets the default save game folder windows configure in their SDK.
        ///
        /// In windows, the path is commonly created at:
        /// C:\Users\username\Saved Games\[rootFolder]\[gameName]
        ///
        /// In linux and similar systems, it gets the user home directory as base and creates a hidden folder starting with a dot.
        ///
        /// In unix, the path is commonly created at:
        /// /home/username/.[rootFolder]/[gameName]
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// std::string path_to_save_files = PlatformPath::getSaveGamePath( "Company Root Folder", "Game Name" );
        /// ...
        /// // Save some data on the path
        /// FILE *out = fopen( ( path_to_save_files + PlatformPath::SEPARATOR + "out.cfg" ).c_str(), "wb" );
        /// if ( out ) {
        ///     ...
        ///     fclose(out);
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param rootFolder base path used to group several games
        /// \param gameName the game name
        /// \return true if its OK
        ///
        static std::string getSaveGamePath(const std::string &rootFolder, const std::string &gameName);

        /// \brief Get and create the application path based to a root folder and app name.
        ///
        /// In windows, the code gets the default documents folder.
        ///
        /// In windows, the path is commonly created at:
        /// C:\Users\username\Documents\[rootFolder]\[appName]
        ///
        /// In linux and similar systems, it gets the user home directory as base and creates a hidden folder starting with a dot.
        ///
        /// In unix, the path is commonly created at:
        /// /home/username/.[rootFolder]/[appName]
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// std::string path_to_save_files = PlatformPath::getDocumentsPath( "Company Root Folder", "App Name" );
        /// ...
        /// // Save some data on the path
        /// FILE *out = fopen( ( path_to_save_files + PlatformPath::SEPARATOR + "out.cfg" ).c_str(), "wb" );
        /// if ( out ) {
        ///     ...
        ///     fclose(out);
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param rootFolder base path used to group several apps
        /// \param appName the app name
        /// \return true if its OK
        ///
        static std::string getDocumentsPath(const std::string &rootFolder, const std::string &appName);

        /// \brief Check if the path is a directory
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// if ( PlatformPath::isDirectory( "directory to test" ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param path the path to test
        /// \return true if it is a directory
        ///
        static bool isDirectory(const std::string &path);

        /// \brief Check if the path is a file
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// if ( PlatformPath::isFile( "file to test" ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param path the path to test
        /// \return true if it is a file
        ///
        static bool isFile(const std::string &path);
        
        
        /// \brief Split the path+file string into path file extension
        ///
        /// Make easy work with path input from main argument
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// int main(int argc, char *argv[]) {
        ///     if ( argc < 2 )
        ///         return -1;
        ///     std::string filepath = std::string(argv[1]);
        ///
        ///     std::string folder, filename, filename_wo_ext, fileext;
        ///     PlatformPath::splitPathString(filepath, &folder, &filename, &filename_wo_ext, &fileext);
        ///
        ///     fprintf(stdout, "Folder: %s\n", folder.c_str());
        ///     fprintf(stdout, "Filename: %s\n", filename.c_str());
        ///     fprintf(stdout, "Filename W/O ext: %s\n", filename_wo_ext.c_str());
        ///     fprintf(stdout, "Filename ext: %s\n", fileext.c_str());
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param input the arg string from main parameter
        /// \param outFolder returns folder path
        /// \param outFilename returns filename with extension
        /// \param outFileWOExt returns filename without extension
        /// \param outFileExt returns extension from filename
        static void splitPathString(const std::string &input, std::string *outFolder, std::string *outFilename, std::string *outFileWOExt, std::string *outFileExt);

    };

}

#endif
