#include "coordinate.h"

#include "vtkMarchingCubesTriangleCases.h"

#include <unordered_map>

template<typename T>
void marching_cubes(std::unordered_map<floatCoordinate, int> & points, QVector<unsigned int> & faces, std::size_t & idCounter, const std::vector<T> & data, const T value
    , const std::array<double, 3> & origin, const std::array<double, 3> & dims, const std::array<double, 3> & spacing, const std::array<double, 6> & extent) {
    const auto triCases = vtkMarchingCubesTriangleCases::GetCases();

    static const int CASE_MASK[8]{1,2,4,8,16,32,64,128};
    static const int edges[12][2]{{0,1}, {1,2}, {3,2}, {0,3}, {4,5}, {5,6}, {7,6}, {4,7}, {0,4}, {1,5}, {3,7}, {2,6}};

    const auto rowSize = dims[0];
    const auto sliceSize = rowSize * dims[1];

    for (std::size_t z = 0; z < (dims[2] - 1); ++z) {
        const auto zOffset = z * sliceSize;
        std::array<std::array<double, 3>, 8> pts;
        pts[0][2] = origin[2] + (z + extent[4]) * spacing[2];
        const auto znext = pts[0][2] + spacing[2];
        for (std::size_t y = 0; y < (dims[1] - 1); ++y) {
            const auto yOffset = y * rowSize;
            pts[0][1] = origin[1] + (y + extent[2]) * spacing[1];
            const auto ynext = pts[0][1] + spacing[1];
            for (std::size_t x = 0; x < (dims[0] - 1); ++x) {
                // get scalar values
                const auto idx = x + yOffset + zOffset;
                std::array<float, 8> cubeVals;
                cubeVals[0] = data[idx];
                cubeVals[1] = data[idx+1];
                cubeVals[2] = data[idx+1 + dims[0]];
                cubeVals[3] = data[idx + dims[0]];
                cubeVals[4] = data[idx + sliceSize];
                cubeVals[5] = data[idx+1 + sliceSize];
                cubeVals[6] = data[idx+1 + dims[0] + sliceSize];
                cubeVals[7] = data[idx + dims[0] + sliceSize];

                // create voxel points
                pts[0][0] = origin[0] + (x + extent[0]) * spacing[0];
                const auto xnext = pts[0][0] + spacing[0];

                pts[1][0] = xnext;
                pts[1][1] = pts[0][1];
                pts[1][2] = pts[0][2];

                pts[2][0] = xnext;
                pts[2][1] = ynext;
                pts[2][2] = pts[0][2];

                pts[3][0] = pts[0][0];
                pts[3][1] = ynext;
                pts[3][2] = pts[0][2];

                pts[4][0] = pts[0][0];
                pts[4][1] = pts[0][1];
                pts[4][2] = znext;

                pts[5][0] = xnext;
                pts[5][1] = pts[0][1];
                pts[5][2] = znext;

                pts[6][0] = xnext;
                pts[6][1] = ynext;
                pts[6][2] = znext;

                pts[7][0] = pts[0][0];
                pts[7][1] = ynext;
                pts[7][2] = znext;

                std::size_t index = 0;
                // Build the case table
                for (std::size_t pi = 0; pi < 8; ++pi) {
                    // for discrete marching cubes, we are looking for an
                    // exact match of a scalar at a vertex to a value
                    if (cubeVals[pi] == value) {
                        index |= CASE_MASK[pi];
                    }
                }
                if (index == 0 || index == 255) {// no surface
                    continue;
                }

                const auto & triCase = triCases[index];
                for (auto edge = triCase.edges; edge[0] > -1; edge += 3) {
                    std::array<std::size_t, 3> ptIds;
                    for (std::size_t vi = 0; vi < 3; vi++) {// insert triangle
                        const auto vert = edges[edge[vi]];
                        // for discrete marching cubes, the interpolation point is always 0.5.
                        const auto t = 0.5;
                        const auto x1 = pts[vert[0]];
                        const auto x2 = pts[vert[1]];
                        std::array<float, 3> vertex;
                        vertex[0] = x1[0] + t * (x2[0] - x1[0]);
                        vertex[1] = x1[1] + t * (x2[1] - x1[1]);
                        vertex[2] = x1[2] + t * (x2[2] - x1[2]);
                        floatCoordinate coord{vertex[0], vertex[1], vertex[2]};
                        if (points.find(coord) == std::end(points)) {
                            points[coord] = ptIds[vi] = idCounter++;// add point
                        } else {
                            ptIds[vi] = points[coord];
                        }
                    }
                    if (ptIds[0] != ptIds[1] && ptIds[0] != ptIds[2] && ptIds[1] != ptIds[2] ) {// check for degenerate triangle
                        faces.push_back(ptIds[0]);
                        faces.push_back(ptIds[1]);
                        faces.push_back(ptIds[2]);
                    }
                }
            }
        }
    }
}

#include "segmentation/cubeloader.h"
#include "segmentation/segmentation.h"
#include "loader.h"
#include "skeleton/skeletonizer.h"

#include <QApplication>
#include <QProgressDialog>

#include <snappy.h>

void generateMeshForSubobjectID(const std::uint64_t value, const Loader::Worker::SnappyCache & cubes, QProgressDialog & progress) {
    std::unordered_map<floatCoordinate, int> points;
    QVector<unsigned int> faces;
    std::size_t idCounter{0};

    for (const auto & pair : cubes) {
        if (progress.wasCanceled()) {
            break;
        }
        const std::size_t cubeEdgeLen = Dataset::current().cubeEdgeLength;
        const std::size_t size = std::pow(cubeEdgeLen, 3);

//        std::array<std::vector<std::uint64_t>, 27> extractedCubes{};// local lookup
//        auto extractedCubeForCoord = [&pair, &extractedCubes](const auto & coord) -> decltype(extractedCubes)::value_type & {
//            const auto ref = pair.first - CoordOfCube{1, 1, 1};
//            const auto diff = coord - ref;
//            return extractedCubes[diff.x * 4 + diff.y * 2 + diff.z];
//        };
//        auto & cube = extractedCubeForCoord(pair.first);
//        cube.resize(size);
//        if (!snappy::RawUncompress(pair.second.c_str(), pair.second.size(), reinterpret_cast<char *>(cube.data()))) {
//            continue;
//        }

        std::unordered_map<CoordOfCube, std::vector<std::uint64_t>> extractedCubes;
        extractedCubes[pair.first].resize(size);
        if (!snappy::RawUncompress(pair.second.c_str(), pair.second.size(), reinterpret_cast<char *>(extractedCubes[pair.first].data()))) {
            continue;
        }

        const auto cubeCoord = Dataset::current().scale.componentMul(pair.first.cube2Global(cubeEdgeLen, 1));

        const std::array<double, 3> dims{{cubeEdgeLen * 1.0, cubeEdgeLen * 1.0, cubeEdgeLen * 1.0}};
        const std::array<double, 3> origin{{cubeCoord.x, cubeCoord.y, cubeCoord.z}};
        const std::array<double, 3> spacing{{Dataset::current().scale.x, Dataset::current().scale.y, Dataset::current().scale.z}};
        const std::array<double, 6> extent{{0, dims[0], 0, dims[1], 0, dims[2]}};

//        marching_cubes(points, faces, idCounter, cube, value, origin, dims, spacing, extent);
        marching_cubes(points, faces, idCounter, extractedCubes[pair.first], value, origin, dims, spacing, extent);
        for (std::size_t i = 0; i < 6; ++i) {
            const std::array<double, 3> dims{{i < 2 ? 2.0 : cubeEdgeLen + 2, i % 4 < 2 ? cubeEdgeLen + 2 : 2.0, i < 4 ? cubeEdgeLen + 2: 2.0}};
            const floatCoordinate unscaledOrigin(pair.first.cube2Global(cubeEdgeLen, 1) + floatCoordinate(i == 0 ? -1 : i == 1 ? 128 : -1, i == 2 ? -1 : i == 3 ? 128 : -1, i == 4 ? -1 : i == 5 ? 128 : -1));
            const std::array<double, 6> extent{{0, dims[0], 0, dims[1], 0, dims[2]}};

            std::vector<std::uint64_t> data(2 * std::pow(cubeEdgeLen + 2, 2));
            const auto rowSize = dims[0];
            const auto sliceSize = rowSize * dims[1];
            for (std::size_t z = 0; z < dims[2]; ++z) {
                for (std::size_t y = 0; y < dims[1]; ++y) {
                    for (std::size_t x = 0; x < dims[0]; ++x) {
                        const Coordinate globalPos = unscaledOrigin + Coordinate(x, y, z);
                        if (globalPos.x < 0 || globalPos.y < 0 || globalPos.z < 0) {
                            continue;
                        }
                        const CoordOfCube coord = globalPos.cube(cubeEdgeLen, 1);
                        const auto inCube = globalPos.insideCube(cubeEdgeLen, 1);
//                        auto & cube = extractedCubeForCoord(coord);
                        auto & cube = extractedCubes[coord];
                        if (cube.empty()) {
                            cube.resize(size);
                            auto findIt = cubes.find(coord);
                            if (findIt == std::end(cubes) || !snappy::RawUncompress(findIt->second.c_str(), findIt->second.size(), reinterpret_cast<char *>(cube.data()))) {
                                std::fill(std::begin(cube), std::end(cube), 0);
                            }
                        }
                        data[z * sliceSize + y * rowSize + x] = boost::multi_array_ref<uint64_t, 3>(reinterpret_cast<uint64_t *>(cube.data()), boost::extents[cubeEdgeLen][cubeEdgeLen][cubeEdgeLen])[inCube.z][inCube.y][inCube.x];
                    }
                }
            }
            const auto scaledOrigin = Dataset::current().scale.componentMul(unscaledOrigin);
            marching_cubes(points, faces, idCounter, data, value, {{scaledOrigin.x, scaledOrigin.y, scaledOrigin.z}}, dims, spacing, extent);
        }
        progress.setValue(progress.value() + 1);
    }
    QVector<float> verts(3 * points.size());
    for (auto && pair : points) {
        verts[3 * pair.second    ] = pair.first.x;
        verts[3 * pair.second + 1] = pair.first.y;
        verts[3 * pair.second + 2] = pair.first.z;
    }
    QVector<float> normals;
    QVector<std::uint8_t> colors;
    Skeletonizer::singleton().addMeshToTree(value, verts, normals, faces, colors, GL_TRIANGLES);

    qDebug() << value << ':' << points.size() << "→" << faces.size() / 3;
}

void generateMeshesForFirstSubobjectsOfSelectedObjects() {
    const auto & cubes = Loader::Controller::singleton().getAllModifiedCubes();
    QProgressDialog progress(QObject::tr("Generating Meshes …"), "Cancel", 0, Segmentation::singleton().selectedObjectsCount() * cubes[0].size(), QApplication::activeWindow());
    progress.setWindowModality(Qt::WindowModal);
    qDebug() << "Generating meshes for" << Segmentation::singleton().selectedObjectsCount() << "objects over" << cubes[0].size() << "cubes";
    for (const auto & objectIndex : Segmentation::singleton().selectedObjectIndices) {
        generateMeshForSubobjectID(Segmentation::singleton().objects[objectIndex].subobjects.front().get().id, cubes[0], progress);
    }
}