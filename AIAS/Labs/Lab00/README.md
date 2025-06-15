# Error1
```
docker: Error response from daemon: Ports are not available: exposing port TCP 0.0.0.0:5000 -> 127.0.0.1:0: listen tcp 0.0.0.0:5000: bind: address already in use.
```
- Cause by Macbook Air M1 2020 Sonoma 14.3
- `lsof -i :5000` or `netstat -tuln | grep 5000` : To check what application used `port 5000`
    - Port 5000 already been used by `ControlCenter`

## Solution
- Change bind `5000:5000` $\to$ `5001:5000` in `./run` file
```
    if [[ "$(docker ps | grep "${CONTAINER_NAME}")" == "" ]]; then
        stop_docker_container
        docker run -d \
            -e USERID="$(id -u)" \
            -e GROUPID="$(id -g)" \
            -p 8888:8888 \
            -p 10000:10000 \
            -p 5173:5173 \
            -p 5001:5000 \
            -v "$([[ ${OSTYPE} == "msys" ]] && echo "/${VOLUME_PROJECTS_DIR}" || echo "${VOLUME_PROJECTS_DIR}")":"/home/${USERNAME}/projects/" \
            -v "$([[ ${OSTYPE} == "msys" ]] && echo "/${VOLUME_SSH_DIR}" || echo "${VOLUME_SSH_DIR}")":"/home/${USERNAME}/.ssh/" \
            -v "$([[ ${OSTYPE} == "msys" ]] && echo "/${VOLUME_VSCODESERVER_DIR}" || echo "${VOLUME_VSCODESERVER_DIR}")":"/home/${USERNAME}/.vscode-server/" \
            --hostname "$(echo "${CONTAINER_NAME}" | tr '[:lower:]' '[:upper:]')" \
            --name "${CONTAINER_NAME}" \
            "${IMAGE_NAME}" ||
            { echo "error due to docker run image" && exit 1; }
    fi
```