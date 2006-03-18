
class CompilerConfig
    attr_accessor :cc, :cxx
    #{{{
    def initialize
        @cflags  = Array.new
        @libs    = Array.new
        @libdirs = Array.new
        @incdirs = Array.new
        @cc      = "/usr/bin/gcc"
        @cxx     = "/usr/bin/g++"
    end
    #}}}
    #{{{
    def normalize(arg, option = nil)
        rv = ''
        if arg.kind_of? Array
            rv = arg.flatten
        elsif arg.kind_of? Rant::FileList
            rv = arg.to_ary.flatten
        elsif arg.kind_of? String
            rv = arg.strip.split
        else
            raise "Unsupported type #{arg.class} in CompilerConfig::normalize"
        end

        if option
            rv.map! { |item|
                if not item.match(/^#{option}/)
                    item = "#{option}#{item}"
                end
                item
            }
        end
        rv
    end
    #}}}
    #{{{
    def add_cflags(cflags)
        @cflags << normalize(cflags)
        @cflags.flatten!.uniq!
    end
    #}}}
    #{{{
    def add_incdirs(incdirs)
        @incdirs << normalize(incdirs, "-I")
        @incdirs.flatten!.uniq!
    end
    #}}}
    #{{{
    def add_libs(libs)
        @libs << normalize(libs, "-l")
        @libs.flatten!.uniq!
    end
    #}}}
    #{{{
    def add_libdirs(libdirs)
        @libdirs << normalize(libdirs, "-L")
        @libdirs.flatten!.uniq!
    end
    #}}}

    #{{{
    def libs
        @libs.join(" ")
    end
    #}}}
    #{{{
    def cflags
        @cflags.join(" ")
    end
    #}}}
    #{{{
    def libdirs
        @libdirs.join(" ")
    end
    #}}}
    #{{{
    def incdirs
        @incdirs.join(" ")
    end
    #}}}
end
