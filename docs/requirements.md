# Requirements

Each requirement has a unique identifier. Every requirement is verified by at least one
test (automated when possible). The name of each automated test suite is the identifier
of the requirement it verifies (see "Traceability").

| ID      | Requirement                                                                                          | Verification        | Status   |
|---------|------------------------------------------------------------------------------------------------------|---------------------|----------|
| REQ-001 | The application shall load a valid binary or ASCII STL file and report its number of triangles.      | Unit test           | Planned  |
| REQ-002 | The application shall reject a malformed or empty STL file with an explicit error, without crashing. | Unit test           | Planned  |
| REQ-003 | The application shall display the loaded model in a 3D view.                                         | Manual test         | Planned  |
| REQ-004 | The user shall be able to rotate, pan and zoom the 3D view with the mouse.                           | Manual test         | Planned  |
| REQ-005 | The application shall compute the distance between two points, equal to the analytical value within 1e-6 model units. | Unit test | Planned  |
| REQ-006 | The `core` library shall build and be tested without any Qt dependency.                              | Build configuration | Verified |
| REQ-007 | The project shall build and pass all automated tests on Windows and Linux in continuous integration. | CI pipeline         | Verified |
| REQ-008 | The application shall expose its version number.                                                     | Unit test           | Verified |

## Traceability

- One automated test suite per requirement, named after its identifier without the hyphen
  (for example `Req001` for REQ-001).
- Each test inside the suite describes one verified behavior.
- Example: `TEST(Req008, ReturnsCurrentVersion)` verifies REQ-008.
- To run only the tests of one requirement: `ctest -R Req008`.

## Status values

- **Planned**: not implemented yet.
- **Implemented**: implemented, verification not yet complete.
- **Verified**: implemented and verified by the stated method.