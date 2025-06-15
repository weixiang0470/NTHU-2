# MobaXterm on MAC
1. Download Xquartz
    - `brew uninstall --cask xquartz`
2. Allow connections from clients
    - XQuartz Preferences → Security → ✔ Allow connections
3. Download Tabby and open X11 forwarding

4. Link to X11 
```
export DISPLAY=:0
xhost +localhost
```

5. Use `sshfs` to mount server on host

# Source
- `source /usr/cad/synopsys/CIC/pa_virtualizer.cshrc`
- `source /home/tools/others/setup_systemc.csh 2.3.1`

# pct
- `cd <your_directory>/PA3`
- `pct&`

# Workstation
- scc is Synopsys’ platform-specific packaging compiler.
- sim-elab is an intermediate file generated during the elaboration stage.
- sim is the final simulation executable.
- After connecting block diagram, run simulation.
    - The entire SystemC project is compiled within the interactive shell of vpsession in Virtualizer.
    - Use Synopsys' packaged scc command along with the GNU g++ compiler.

- ::scsh::open-project
- ::scsh::cosim::enable_hdl_sdi
- ::scsh::build-options -skip-elab on
    - Skip elaboration and build directly.
- ::set_maf mem_map
- ::scsh::build
    - Generate sim.exe and prepare for simulation.

