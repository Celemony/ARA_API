//------------------------------------------------------------------------------
//! \file       ARACLAP.h
//!             Celemony extension to the CLAP SDK to integrate ARA into CLAP plug-ins
//! \project    ARA API Specification
//! \copyright  Copyright (c) 2022-2023, Celemony Software GmbH, All Rights Reserved.
//!             Developed in cooperation with Timo Kaluza (defiantnerd)
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

#ifndef ARACLAP_h
#define ARACLAP_h

#include "ARAInterface.h"

#include "clap/clap.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//! @defgroup Companion_APIs Companion APIs
//! @{

//! @defgroup CLAP CLAP
//! @{

//! Factory ID for retrieving the clap_ara_factory_t extension from clap_plugin_entry_t.get_factory()
ARA_DRAFT static CLAP_CONSTEXPR const char CLAP_EXT_ARA_FACTORY[] = "org.ara-audio.ara.factory.draft/2";

//! Extension ID for retrieving the clap_ara_plugin_extension_t from clap_plugin_t.get_extension()
ARA_DRAFT static CLAP_CONSTEXPR const char CLAP_EXT_ARA_PLUGINEXTENSION[] = "org.ara-audio.ara.pluginextension.draft/2";

//! Add this feature if your plugin supports ARA.
//! This allows hosts to detect ARA early on in the setup phase.
#define /*ARA_DRAFT*/ CLAP_PLUGIN_FEATURE_ARA_SUPPORTED "ara:supported"

//! Add this feature if your plugin requires ARA to operate (will not work as normal insert plug-in).
//! This allows non-ARA CLAP hosts to suppress the plug-in since it cannot be used there.
#define /*ARA_DRAFT*/ CLAP_PLUGIN_FEATURE_ARA_REQUIRED "ara:required"


/***************************************************************************************************/
//! Extension interface to connect to ARA at CLAP factory level.
//! The host can pass CLAP_EXT_ARA_FACTORY to clap_plugin_entry_t.get_factory() to directly obtain an
//! ARAFactory, which allows for creating and maintaining the model independently of any clap_plugin_t
//! instances, enabling tasks such as automatic tempo detection or audio-to-MIDI conversion.
//! For rendering and editing the model however, there must be an associated clap_plugin_t provided in
//! the same binary - the descriptor for which is returned at the same index as the related ARAFactory.

ARA_DRAFT typedef struct clap_ara_factory
{
    //! Get the number of ARA factories (i.e. ARA-capable plug.-ins) available.
    //! Note that the regular clap_plugin_factory can contain more plug-ins if these do not support
    //! ARA - make no assumption about items returned here being related to the items returned there
    //! in terms of count or order.
    uint32_t(CLAP_ABI *get_factory_count)(const struct clap_ara_factory *factory);

    //! Get the ARA factory for the plug-in at the given index.
    //! The returned pointer must remain valid until clap_plugin_entry_t.deinit() is called.
    //! The returned ARAFactory must be equal to the ARAFactory returned from instances of the
    //! associated CLAP plug-in through their clap_ara_plugin_extension_t.get_factory().
    const ARA_NAMESPACE ARAFactory *(CLAP_ABI *get_ara_factory)(const struct clap_ara_factory *factory, uint32_t index);

    //! Get the ID of the CLAP plug-in associated with the ARA factory for the given index.
    //! The plug-in must be in the same binary.
    //! The returned pointer must remain valid until clap_plugin_entry_t.deinit is called.
    const char *(CLAP_ABI *get_plugin_id)(const struct clap_ara_factory *factory, uint32_t index);
} clap_ara_factory_t;


/***************************************************************************************************/
//! Extension interface to connect to ARA at CLAP plug-in level.
//! This interface provides access to the ARA specific extension of a CLAP plug-in.
//! Return an pointer to a clap_ara_plugin_extension_t when clap_plugin_t.get_extension() is called
//! with CLAP_EXT_ARA_PLUGINEXTENSION.

ARA_DRAFT typedef struct clap_ara_plugin_extension
{
    //! Access the ARAFactory associated with this plug-in.
    const ARA_NAMESPACE ARAFactory *(CLAP_ABI *get_factory)(const clap_plugin_t *plugin);

    //! Bind the CLAP instance to an ARA document controller, switching it from "normal" operation
    //! to ARA mode with the assigned roles, and exposing the ARA plug-in extension.
    //! \p knownRoles encodes all roles that the host considered in its implementation and will explicitly
    //! assign to some plug-in instance(s), while \p assignedRoles describes the roles that this specific
    //! instance will fulfill.
    //! This may be called only once during the lifetime of the CLAP plug-in, before the first call
    //! to clap_plugin_t.activate() or clap_host_state_t.load() or other processing related extensions
    //! or the creation of the GUI.
    //! The ARA document controller must remain valid as long as the plug-in is in use - rendering,
    //! showing its UI, etc. However, when tearing down the plug-in, the actual order for deleting
    //! the clap_plugin_t instance and for deleting ARA document controller is undefined.
    //! Plug-ins must handle both potential destruction orders to allow for a simpler reference
    //! counting implementation on the host side.
    const ARA_NAMESPACE ARAPlugInExtensionInstance *(CLAP_ABI *bind_to_document_controller)
                                                        (const clap_plugin_t                     *plugin,
                                                         ARA_NAMESPACE ARADocumentControllerRef   documentControllerRef,
                                                         ARA_NAMESPACE ARAPlugInInstanceRoleFlags knownRoles,
                                                         ARA_NAMESPACE ARAPlugInInstanceRoleFlags assignedRoles);
} clap_ara_plugin_extension_t;

//! @}

//! @}

#if defined(__cplusplus)
}   // extern "C"
#endif

#endif // ARACLAP_h
