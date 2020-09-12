ARG  DOCKER_IMAGE
ARG  IMAGE_VERSION=latest
FROM ${DOCKER_IMAGE}:${IMAGE_VERSION}

ENV PROJECT_DIR /project
ENV PROJECT_SOURCE_DIR ${PROJECT_DIR}/source
ENV PROJECT_BUILD_DIR ${PROJECT_DIR}/build
ADD . ${PROJECT_SOURCE_DIR}

RUN cmake -DCMAKE_BUILD_TYPE=Release -S ${PROJECT_SOURCE_DIR} -B ${PROJECT_BUILD_DIR}
WORKDIR ${PROJECT_BUILD_DIR}
RUN make

WORKDIR ${PROJECT_DIR}/test_files
RUN ls
CMD ["/project/build/test_executable", "&& cat keyword.txt", "&& cat text.txt", "&& cat result.txt"]
