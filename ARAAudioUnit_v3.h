//------------------------------------------------------------------------------
//! \file       ARAAudioUnit_v3.h
//!             integrates ARA into Audio Unit v3 App Extension plug-ins
//! \project    ARA API Specification
//! \copyright  Copyright (c) 2021-2023, Celemony Software GmbH, All Rights Reserved.
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

#ifndef ARAAudioUnit_v3_h
#define ARAAudioUnit_v3_h

#include "ARAInterface.h"

#import <AudioUnit/AUAudioUnit.h>

#if defined(__GNUC__)
    _Pragma ("GCC diagnostic push")
    _Pragma ("GCC diagnostic ignored \"-Wpedantic\"")
#endif
#warning "AudioUnit v3 App Extension support is still work-in-progress and will change in potentially incompatible ways (e.g. out-of-process loading)"
#if defined(__GNUC__)
    _Pragma ("GCC diagnostic pop")
#endif


//! @defgroup Companion_APIs Companion APIs
//! @{

/***************************************************************************************************/
//! @defgroup Audio_Unit_v3 Audio Unit V3
//! Experimental support for Audio Unit v3 (App Extension)
//! @{

//! Integration with the system-wide Audio Unit cache.
//! To allow hosts to defer opening any Audio Unit v3 App Extensions until the user explicitly
//! requests it, the system caches various information about the Audio Unit.
//! To support this optimization also when ARA is used, plug-ins must be marked as ARA-compatible
//! at the Audio Component level inside its NSExtensionAttributes by including the tag "ARA" in
//! its Info.plist file, like in this example:
//! \code{.xml}
//! <key>NSExtension</key>
//! <dict>
//! <key>NSExtensionAttributes</key>
//!     <dict>
//!         <key>AudioComponentBundle</key>
//!         <string>your.identifier.goes.here</string>
//!         <key>AudioComponents</key>
//!         <array>
//!             <dict>
//!                 ...
//!                 <key>manufacturer</key>
//!                 <string>Demo</string>
//!                 <key>subtype</key>
//!                 <string>Test</string>
//!                  <key>type</key>
//!                 <string>aufx</string>
//!                 ...
//!                 <key>tags</key>
//!                 <array>
//!                     <string>Effects</string>
//!                     ...
//!                     <string>ARA</string>
//!                 </array>
//!             </dict>
//!             ...
//! \endcode
#define /*ARA_DRAFT*/ kARAAudioComponentTag "ARA"


//! Protocol to be implemented by ARA-compatible subclasses AUAudioUnit.
//! This protocol may or may not be replaced with the AUMessageChannel based communication defined below.
ARA_DRAFT @protocol ARAAudioUnit <NSObject>

@required
#if defined(__cplusplus)
- (nonnull const ARA::ARAFactory *) araFactory;

- (nonnull const ARA::ARAPlugInExtensionInstance *) bindToDocumentController:(nonnull ARA::ARADocumentControllerRef) documentControllerRef
                                                    withRoles:(ARA::ARAPlugInInstanceRoleFlags) assignedRoles
                                                    knownRoles:(ARA::ARAPlugInInstanceRoleFlags) knownRoles;
#else
//! Get the ARA factory.
//! The returned pointer must remain valid throughout the lifetime of the App Extension that contains
//! the AUAudioUnit.
- (nonnull const ARAFactory *) araFactory;

//! Bind the AUAudioUnit instance to an ARA document controller, switching it from "normal" operation
//! to ARA mode, and exposing the ARA plug-in extension.
//! \p knownRoles encodes all roles that the host considered in its implementation and will explicitly
//! assign to some plug-in instance(s), while \p assignedRoles describes the roles that this specific
//! instance will fulfill.
//! This may be called only once during the lifetime of the AUAudioUnit instance, before
//! the first call to allocateRenderResourcesAndReturnError: or writing to fullState,
//! fullStateForDocument or currentPreset, or the creation of the GUI (see NSExtensionRequestHandling).
//! The ARA document controller must remain valid as long as the plug-in is in use - rendering,
//! showing its UI, etc. However, when tearing down the plug-in, the actual order for deleting
//! the AUAudioUnit instance and for deleting ARA document controller is undefined.
//! Plug-ins must handle both potential destruction orders to allow for a simpler reference
//! counting implementation on the host side.
- (nonnull const ARAPlugInExtensionInstance *) bindToDocumentController:(nonnull ARADocumentControllerRef) documentControllerRef
                                               withRoles:(ARAPlugInInstanceRoleFlags) assignedRoles
                                               knownRoles:(ARAPlugInInstanceRoleFlags) knownRoles;
#endif

@end


//! UTIs for ARA message protocols used for [AUAudioUnit messageChannelFor:] (added in macOS 13)
//! The message channel for ARA_AUDIOUNIT_DOCUMENTCONTROLLER_CUSTOM_MESSAGES_UTI should only be
//! obtained and configured once, it will also be used for all document controller communications
//! based on the factories.
//! The message channel for ARA_AUDIOUNIT_PLUGINEXTENSION_CUSTOM_MESSAGES_UTI on the other hand
//! should be managed per ARA-enabled Audio Unit plug-in instance.
//! This API may eventually replace the above ARAAudioUnit <NSObject> draft.
//@{
#define ARA_AUDIOUNIT_FACTORY_CUSTOM_MESSAGES_UTI /*ARA_DRAFT*/ @"org.ara-audio.factory"
#define ARA_AUDIOUNIT_PLUGINEXTENSION_CUSTOM_MESSAGES_UTI /*ARA_DRAFT*/ @"org.ara-audio.pluginextension"
//@}


//! @}

//! @}

#endif // ARAAudioUnit_v3_h
