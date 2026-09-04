"""Small source grouping helper for SAGA's C/C++ builds."""

load("@rules_cc//cc:cc_library.bzl", "cc_library")

def saga_component_libraries(
        name_prefix,
        engine_dirs,
        root_sources,
        sources,
        headers,
        textual_headers,
        copts,
        defines,
        deps,
        target_compatible_with = []):
    """Create one archive per existing top-level source component."""
    components = [("root", root_sources + [source for source in sources if source.startswith("host/")])]
    components += [
        (directory.lower(), [source for source in sources if source.startswith(directory + "/")])
        for directory in engine_dirs
    ]

    labels = []
    for component, component_sources in components:
        name = name_prefix + "_" + component
        cc_library(
            name = name,
            srcs = component_sources,
            hdrs = headers,
            copts = copts,
            defines = defines,
            includes = ["."],
            linkstatic = True,
            local_defines = ["__FILENAME__=__FILE__"],
            target_compatible_with = target_compatible_with,
            textual_hdrs = textual_headers,
            deps = deps,
            alwayslink = True,
        )
        labels.append(":" + name)
    return labels
