//------------------------------------------------------------------------------
//! \file       ARAAudioFileChunks.h
//!             definition of the audio file chunks related to ARA partial persistency
//! \project    ARA API Specification
//! \copyright  Copyright (c) 2018-2021, Celemony Software GmbH, All Rights Reserved.
//!             Developed in cooperation with PreSonus Software Ltd.
//! \license    Licensed under the Apache License, Version 2.0 (the "License");
//!             you may not use this file except in compliance with the License.
//!             You may obtain a copy of the License at
//!
//!               http://www.apache.org/licenses/LICENSE-2.0
//!
//!             Unless required by applicable law or agreed to in writing, software
//!             distributed under the License is distributed on an "AS IS" BASIS,
//!             WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//!             See the License for the specific language governing permissions and
//!             limitations under the License.
//------------------------------------------------------------------------------


#ifndef ARAAudioFileChunks_h
#define ARAAudioFileChunks_h

#include "ARAInterface.h"

#if defined(__cplusplus) && !(ARA_DOXYGEN_BUILD)
namespace ARA
{
extern "C"
{
#endif

/***************************************************************************************************/
//! @defgroup ARAAudioFileChunks ARA Audio File Chunks
//!
//! To exchange persistent ARA audio source state information between compatible ARA applications,
//! ARA 2.0 defines a format for embedding these states into standardized audio file chunks.
//! From there, they can be imported into any ARA document using @ref Partial_Document_Persistency.
//! It is possible to store this data for multiple plug-ins, for example if some sample content
//! provider wants to ship the content with properly validated audio source analysis for various
//! plug-ins readily included.
//! Other use cases include migrating data from one plug-in to another, or adding ARA objects to a
//! host document via drag and drop from a plug-in that ships with a built-in sound library.
//! Note that after loading the data, ARA content readers can be used to extract more information
//! about the audio source, such as tempo map, time and key signatures, etc.
//! The ARA chunk should be evaluated both when adding a new audio file to the arrangement and when
//! applying a new ARA plug-in for an audio file already used in the arrangement.
//!
//! Covering both aif and wav formats, ARA stores its data by extending iXML, see http://www.ixml.info
//! Inside the iXML document, there's a custom tag \<ARA\> that encloses a dictionary of audio source
//! archives, encoded as array tagged \<audioSources\>. Each entry in the array contains the tag
//! \<documentArchiveID\> which also functions as the key for the dictionary, and associated data
//! which includes the actual binary archive, for example:
//! \code{.xml}
//! <ARA>
//!     <audioSources>
//!         <audioSource>
//!             <documentArchiveID>com.celemony.ara.audiosourcedescription.13</documentArchiveID>
//!             <openAutomatically>false</openAutomatically>
//!             <suggestedPlugIn>
//!                 <plugInName>Melodyne</plugInName>
//!                 <lowestSupportedVersion>5.0.0</lowestSupportedVersion>
//!                 <manufacturerName>Celemony</manufacturerName>
//!                 <informationURL>https://www.celemony.com</informationURL>
//!             </suggestedPlugIn>
//!             <persistentID>59D4874F-FA5A-4FE8-BAC6-0E8BC5F6184A</persistentID>
//!             <archiveData>TW9pbiBEdQ==</archiveData>
//!         </audioSource>
//!         <!-- ... potentially more archives keyed by different documentArchiveIDs here ... -->
//!     </audioSources>
//! </ARA>
//! \endcode
//! @{

#if defined(__cplusplus)
    //! Name of the XML element that contains the vendor-specific iXML sub-tree for ARA.
    constexpr auto kARAXMLName_ARAVendorKeyword { "ARA" };

    //! Name of the XML element that contains the dictionary of audio source archives inside the ARA sub-tree.
    constexpr auto kARAXMLName_AudioSources { "audioSources" };

    //! Name of each XML element inside the dictionary of audio source archives.
    constexpr auto kARAXMLName_AudioSource { "audioSource" };

    //! Name of the XML element inside an audio source archive that acts as unique dictionary key
    //! for the list of audio source archives and identifies the opaque archive content.
    //! string value, see ARAFactory::documentArchiveID and ARAFactory::compatibleDocumentArchiveIDs.
    constexpr auto kARAXMLName_DocumentArchiveID { "documentArchiveID" };
    //! Name of the XML element inside an audio source archive that indicates whether the host should
    //! immediately load the archive data into a new audio source object and create an audio modification
    //! and playback region for it, or else import the audio file without ARA initially and only load
    //! the ARA archive later on demand when the user manually requests it by adding a matching plug-in.
    //! boolean value ("true" or "false").
    constexpr auto kARAXMLName_OpenAutomatically { "openAutomatically" };
    //! Name of the XML element inside an audio source archive that provides user-readable information
    //! about the plug-in for which the archive was originally created. This can be used for proper
    //! error messages, e.g. if openAutomatically is true but no plug-in compatible with the archive's
    //! given documentArchiveID is installed.
    constexpr auto kARAXMLName_SuggestedPlugIn { "suggestedPlugIn" };
    //! Name of the XML element inside an audio source archive that encodes the persistent ID that
    //! was assigned to the audio source when creating the archive. When loading the archive, the
    //! plug-in will use this persistent ID to find the target object to extract the state to.
    //! string value, see ARAAudioSourceProperties::persistentID, see ARARestoreObjectsFilter.
    constexpr auto kARAXMLName_PersistentID { "persistentID" };
    //! Name of the XML element inside an audio source archive that encodes the actual binary data.
    //! of the archive in Base64 format.
    //! string value, see ARAArchivingControllerInterface, see https://tools.ietf.org/html/rfc4648.
    constexpr auto kARAXMLName_ArchiveData { "archiveData" };

    //! Name of the XML element inside a suggested plug-in element that encodes the plug-in name as string.
    constexpr auto kARAXMLName_PlugInName { "plugInName" };
    //! Name of the XML element inside a suggested plug-in element that encodes the minimum version
    //! of the plug-in that is compatible with this archive as string.
    constexpr auto kARAXMLName_LowestSupportedVersion { "lowestSupportedVersion" };
    //! Name of the XML element inside a suggested plug-in element that encodes the plug-in manufacturer as string.
    constexpr auto kARAXMLName_ManufacturerName { "manufacturerName" };
    //! Name of the XML element inside a suggested plug-in element that encodes the plug-in information URL as string.
    constexpr auto kARAXMLName_InformationURL { "informationURL" };
#else
    #define kARAXMLName_ARAVendorKeyword "ARA"

    #define kARAXMLName_AudioSources "audioSources"

    #define kARAXMLName_AudioSource "audioSource"

    #define kARAXMLName_DocumentArchiveID "documentArchiveID"
    #define kARAXMLName_OpenAutomatically "openAutomatically"
    #define kARAXMLName_SuggestedPlugIn "suggestedPlugIn"
    #define kARAXMLName_PersistentID "persistentID"
    #define kARAXMLName_ArchiveData "archiveData"

    #define kARAXMLName_PlugInName "plugInName"
    #define kARAXMLName_LowestSupportedVersion "lowestSupportedVersion"
    #define kARAXMLName_ManufacturerName "manufacturerName"
    #define kARAXMLName_InformationURL "informationURL"
#endif

//! @}

#if defined(__cplusplus) && !(ARA_DOXYGEN_BUILD)
}   // extern "C"
}   // namespace ARA
#endif

#endif // ARAAudioFileChunks_h
