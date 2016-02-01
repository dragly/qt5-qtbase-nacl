var LibraryExample = {
    // Internal functions
    $EXAMPLE__deps: ["glUniform1f, glUniform4fv"],
    $EXAMPLE: {
        internal_func: function () {
        }
    },
    example_initialize: function (arg) {
        EXAMPLE.internal_func()
    },
    example: function(a) {
        console.log("LOL " + a);
        _glUniform1f(a);
    }
}

autoAddDeps(LibraryExample, '$EXAMPLE')
mergeInto(LibraryManager.library, LibraryExample)
