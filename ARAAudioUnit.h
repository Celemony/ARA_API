//------------------------------------------------------------------------------
//! \file       ARAAudioUnit.h
//!             integrates ARA into Audio Unit plug-ins
//! \project    ARA API Specification
//! \copyright  Copyright (c) 2012-2023, Celemony Software GmbH, All Rights Reserved.
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

#ifndef ARAAudioUnit_h
#define ARAAudioUnit_h

#include "ARAInterface.h"

#include "MacTypes.h"

#if defined(__cplusplus)
namespace ARA
{
extern "C"
{
#endif

#if defined(__clang__)
    _Pragma("GCC diagnostic push")
    _Pragma("GCC diagnostic ignored \"-Wfour-char-constants\"")
#endif

//! @defgroup Companion_APIs Companion APIs
//! @{

/***************************************************************************************************/
//! @defgroup Audio_Unit Audio Unit
//! ARA support for Audio Units is mostly based on private Audio Unit properties. The input/output
//! pattern for the associated property data structs is similar to struct AudioUnitParameterStringFromValue
//! in the Audio Unit API.
//! To be able to detect potential collisions if some host or Audio Unit uses the same private property
//! for non-ARA communication, the actual ARA data is embedded into a property data struct that also
//! contains an ARA magic number. For this magic number to work properly for all host/plug-in combinations,
//! it's crucial that hosts set it on input and check it after output, and that plug-ins check it
//! before writing to the property struct.
//! @{

//! Integration with the system-wide Audio Unit cache.
//! Since the introduction of the Audio Component APIs in macOS 10.10 Yosemite, a system-wide cache
//! of all installed Audio Units allows hosts to defer opening any Audio Unit until the user explicitly
//! requests this. To support this optimization also when ARA is used, plug-ins must be marked as
//! ARA-compatible at the Audio Component level by including the tag "ARA" in their Info.plist file,
//! like in this example:
//! \code{.xml}
//! <key>AudioComponents</key>
//! <array>
//!     <dict>
//!         ...
//!         <key>manufacturer</key>
//!         <string>Demo</string>
//!         <key>subtype</key>
//!         <string>Test</string>
//!         <key>type</key>
//!         <string>aufx</string>
//!         ...
//!         <key>tags</key>
//!         <array>
//!             <string>Effect</string>
//!             ...
//!             <string>ARA</string>
//!         </array>
//!     </dict>
//! </array>
//! \endcode
#define /*ARA_ADDENDUM(2_0_Final)*/ kARAAudioComponentTag   "ARA"

//! Property data for kAudioUnitProperty_ARAFactory.
typedef struct ARAAudioUnitFactory
{
    //! Input/output: token to identify the property is actually used for ARA.
    OSType inOutMagicNumber;

    //! Output: pointer to the factory associated with the Audio Unit.
    const ARAFactory * outFactory;
} ARAAudioUnitFactory;

//! Property data for kAudioUnitProperty_ARAPlugInExtensionBindingWithRoles.
typedef struct ARAAudioUnitPlugInExtensionBinding
{
    //! Input/output: token to identify the property is actually used for ARA.
    OSType inOutMagicNumber;

    //! Input: document controller of the model graph that the Audio Unit shall be bound to.
    ARADocumentControllerRef inDocumentControllerRef;

    //! Output: the entry point for the plug-in.
    const ARAPlugInExtensionInstance * outPlugInExtension;

    //! Input: all roles that the host considered in its implementation and will explicitly
    //! assign to some plug-in instance(s).
    //! Being an ARA 2.0 addition, this field is only available
    //! if kAudioUnitProperty_ARAPlugInExtensionBindingWithRoles is used.
    ARA_ADDENDUM(2_0_Draft) ARAPlugInInstanceRoleFlags knownRoles;

    //! Input: roles assigned to this specific plug-in instance.
    //! Being an ARA 2.0 addition, this field is only available
    //! if kAudioUnitProperty_ARAPlugInExtensionBindingWithRoles is used.
    ARA_ADDENDUM(2_0_Draft) ARAPlugInInstanceRoleFlags assignedRoles;
} ARAAudioUnitPlugInExtensionBinding;

enum
{
    //! This value must be placed in the magicNumber fields of the ARA properties.
    kARAAudioUnitMagic = 'Ara!'
};

enum
{
    //! kAudioUnitScope_Global, read-only, property data is ARAAudioUnitFactory:
    //! query the ARA factory associated with the given plug-in.
    kAudioUnitProperty_ARAFactory = 'AraF'
};
enum ARA_DEPRECATED(2_0_Draft)
{
    //! kAudioUnitScope_Global, read-only, property data is ARAAudioUnitPlugInExtensionBinding:
    //! bind the Audio Unit instance to an ARA document controller, switching it from "normal"
    //! operation to ARA mode, and exposing the ARA plug-in extension.
    //! Note that since ARA 2.0, this property has been deprecated and replaced with
    //! kAudioUnitProperty_ARAPlugInExtensionBindingWithRoles.
    //! This deprecated call is equivalent to the new call with no known roles set, however all
    //! ARA 1.x hosts are in fact using all instances with playback renderer, edit renderer and
    //! editor view role enabled, so plug-ins implementing ARA 1 backwards compatibility can
    //! safely assume those three roles to be enabled if this call was made.
    //! Same call order rules as kAudioUnitProperty_ARAPlugInExtensionBindingWithRoles apply.
    kAudioUnitProperty_ARAPlugInExtensionBinding = 'AraB',
};
enum ARA_ADDENDUM(2_0_Draft)
{
    //! ARA 2 extended version of kAudioUnitProperty_ARAPlugInExtensionBinding:
    //! kAudioUnitScope_Global, read-only, property data is ARAAudioUnitPlugInExtensionBinding:
    //! bind the Audio Unit instance to an ARA document controller, switching it from "normal"
    //! operation to ARA mode with the assigned roles, and exposing the ARA plug-in extension.
    //! This may be done only once during the lifetime of the Audio Unit, before initializing it via
    //! kAudioUnitInitializeSelect or setting its state via the properties ClassInfo, PresentPreset
    //! or CurrentPreset or before creating any of the custom views for Audio Unit.
    //! The ARA document controller must remain valid as long as the plug-in is in use - rendering,
    //! showing its UI, etc. However, when tearing down the plug-in, the actual order for deleting
    //! the Audio Unit and for deleting ARA document controller is undefined.
    //! Plug-ins must handle both potential destruction orders to allow for a simpler reference
    //! counting implementation on the host side.
    kAudioUnitProperty_ARAPlugInExtensionBindingWithRoles = 'AraE'
};

//! @}

//! @}

#if defined(__GNUC__)
    _Pragma("GCC diagnostic pop")
#endif

#if defined(__cplusplus)
}   // extern "C"
}   // namespace ARA
#endif

#endif // ARAAudioUnit_h
