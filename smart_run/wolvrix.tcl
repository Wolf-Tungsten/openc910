proc getenv {name {default ""}} {
    if {[info exists ::env($name)] && $::env($name) ne ""} {
        return $::env($name)
    }
    return $default
}

if {[info exists ::WOLVRIX_SCRIPT_DIR] && $::WOLVRIX_SCRIPT_DIR ne ""} {
    set scriptDir $::WOLVRIX_SCRIPT_DIR
} else {
    set scriptDir [file dirname [file normalize [info script]]]
}
set repoRoot [file normalize [file join $scriptDir ../../../..]]
set buildDir [getenv "BUILD_DIR" [file join $repoRoot build]]
set outputDir [file join $buildDir c910 wolf_emit]
set filelist [file normalize [file join $scriptDir logical filelists sim_wolf.fl]]
set topName "sim_top"
set emittedSv [file join $outputDir "${topName}_wolf.sv"]

set readArgs [list -f $filelist]
lappend readArgs --top $topName

eval read_sv $readArgs

set passList "xmr-resolve const-fold redundant-elim memory-init-check dead-code-elim stats"
foreach pass $passList { transform $pass }

if {[getenv "WOLF_JSON_ROUNDTRIP" "0"] eq "1"} {
    set jsonPath [file join $outputDir "${topName}_wolf.json"]
    write_json -o $jsonPath
    close_design
    read_json $jsonPath
}

write_sv -o $emittedSv
