# Docker image for PrintConcrete

To build a docker image for running the PrintConcrete app:

    docker build . -t <image-name>

where `<image-name>` is your name of choice.
Then to execute the simulator on an input file `mymodel.xinp`:

    docker run --rm -v $PWD:/work <image-name> PrintConcrete mymodel.xinp [<ifem-options>]

where `<ifem-options>` are other command-line options for IFEM apps,
for instance `-hdf5` if you want to export the results to a HDF5-file.
