Microsoft Media Manager1.0
README for Beta 2 release
October 1996
(c) Microsoft Corporation, 1996
____________________________________________________________________

TO VIEW THIS FILE IN WINDOWS NOTEPAD,
      turn on Word wrap (in the Edit menu).
____________________________________________________________________

This file contains important information about how to setup Media Manager, known issues with this release, and where to find additional information.

__________________________________
CONTENTS
    1.0	 Installing Media Manager 1.0
    2.0  Major Features
    3.0  Support for Media Manager
    4.0	 Known Bugs

_____________________________________________ 
1.0  Installing Media Manager 1.0

1.1  Media Manager runs on Windows 95 and Windows NT 4.0. The files occupy less than 8MB of disk space. The minimum configuration: 486-66 with 16MB RAM, CD-ROM drive and VGA graphics card.
1.2  Specify the location of an annotations database. To create a default database on your local drive, click the Create Annotations Database button. You can easily change this later when you determine the address of the shared annotations database.
1.3  Restart Windows to complete the installation. 

_____________________________________________
2.0  Major Features

2.1  Browse, preview and play in-place media files stored in Media Manager folders right from the Windows Shell
2.2  Properties for media files are extracted, indexed and can be viewed in property pages
2.3  Add your own annotations or custom properties to any file in Media Manager
2.4  Powerful Media Manager Finder searches on all properties and contents
2.5  Background Indexer for quick searches


_____________________________________________
3.0  Support for Media Manager

3.1  Help File.  A comprehensive help file is automatically installed with Setup. Help includes tool tips, What's This? context-sensitive help, specific troubleshooting information via Help buttons on user messages, and a complete linked help file available from the Media Manager Finder Help menu.

3.2  Introducing Screen. The Introducing Media Manager screen directs you to an overview of the major Media Manager features.

_____________________________________________
4.0  Known Bugs

I - General

4.1  For Media Manager to function correctly, Media Manager folders must be marked as READ-ONLY at all times. If you accidently change the READ-ONLY flag on a Media Manager folder, its icon will appear as a windows folder icon. To correct this problem for a root Media Manager folder, select the folder, bring up the Properties dialog, check the READ-ONLY flag and refresh the view.  To correct this problem for a Media Manager subfolder,  select the folder and at least one other file, bring up the Properties dialog, check the read-only flag for the selection and refresh the view. 
4.2  For the playable media types such as AVI, Quicktime, or MPEG, the Audio and Video property pages might appear blank. Typically when this occurs, there is something wrong with the file, the correct codec may not be installed, or it may be in a format unsupported by ActiveMovie 1.0. If only one of the pages appears, part of the file could be understood.
4.3  Dragging and dropping folders can cause the Scope Pane to show a different set of folders than is visible in the Results Pane. Hit F5 to refresh the windows.
4.4  Some TGA or JPG files in the property page might display a color depth of 72. This is a problem with Microsoft ActiveMovie 1.0. Media Manager uses ActiveMovie to extract the color depth information for these files.
4.5  If you do not have audio hardware installed on your machine, Media Manager will not be able to extract audio properties from supported audio file types. As a result, you will not see an audio property page for audio file types, the audio property page for supported video files (with audio components) will be empty, and you will not be able to find files using the Media Manager finder based on audio properties. Media Manager uses ActiveMovie 1.0 to extract audio information for the supported file types and ActiveMovie 1.0 requires audio hardware to be installed.
4.6  Media Manager only supports SUBST drives if they are substituted for the root of a drive. If you create a SUBST drive for a directory within a drive you will not be able to search for content via this SUBST drive in the Media Manager finder, or execute indexer operations (create, delete update) via the context menu of a root Media Manager folder on the SUBST drive.


II - Sharing

4.7  If you have a share on an NTFS drive and the Sharing is set to Read/Write only without Change access, you cannot rename Media Manager folders. You will get an error message. If you have Read/Write/Change access, then you can rename the Media Manager folder. Set your access to Read/Write/Change to avoid this problem.
4.8  Using Windows 95 or Windows NT file sharing, you cannot share the root level of a Media Manager folder. If you do, the folder will appear as a normal Windows folder when another machine connects to the shared folder. To avoid this, share a folder above the Media Manager folder.
4.9  To share a Media Manager folder on a server, you can copy the Media Manager folder to a server on the network just as you would any other folder. Only people with Media Manager installed will be able to browse into the folders.  The folders will only be indexed and searchable if Media Manager has been installed on the server.


III - Thumbnail View

4.10  In some instances, you may not hear audio or see video when you play in place a Quicktime or MPEG file. Media Manager used Microsoft ActiveMovie 1.0 to play QUicktime and MPEG files. ActiveMovie 1.0 does not support all variations of Quicktime or MPEG.
4.11 JPEG files may appear playable in thumbnail view. The Apple Quicktime MCI drivers register JPEG as a playable media format. If you have had Apple Quicktime drivers installed on your machine, selecting a JPEG file might display a playbar.


IV - Convert To A Media Manager Folder or Convert to a Windows Folder

4.12 When converting or unconverting a folder, right-click a folder in the Results Pane and choose the appropriate context menu item.  Do not convert or unconvert folders from the Scope Pane.
4.13 Because Media Manager folders are an enhanced version of DOS directories, you can always salvage your data files by using the Windows File Manager, in the unlikely event that a Media Manager folder become corrupted or damaged. To run the File Manager, choose Run  from the Start Menu and enter "Winfile".  You will lose any annotations associated with  the file when it is removed from the Media Manager folder. 
