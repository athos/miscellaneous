#!/bin/bash

java -cp src:conf:lib/jar/macosx/RXTXcomm.jar -Djava.library.path=lib/native/macosx RemoteController
