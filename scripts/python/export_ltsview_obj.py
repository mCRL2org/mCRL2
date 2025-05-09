#!/usr/bin/env python3
"""
export_obj.py - Script to export ltsview text export into a Wavefront .obj file (in text format)
"""

import argparse
import sys
import re
import numpy as np  # Add this import for matrix operations

from dataclasses import dataclass
from typing import Optional, List, Tuple
import math

# Legend:
#  cone(x,y,z): a cone with top radius x, base radius y and height z
#  sphere(x):   a sphere with radius x
#  at angle(x): cluster is on the rim of the base of its parent cluster at angle x
#               If cluster is a cone, the center of its top side is placed there.
#               If cluster is a sphere, the center of the sphere is placed there.
# ---------------------------------------------------------------------------------


@dataclass(frozen=True)
class Cone:
    top_radius: float
    base_radius: float
    height: float


@dataclass(frozen=True)
class Sphere:
    radius: float


@dataclass
class Cluster:
    shape: object  # Can be a Cone or Sphere
    rank: int  # Rank of the cluster
    angle: Optional[float] = None  # Angle in degrees, optional
    parent_id: Optional[int] = None  # ID of the parent cluster, optional


@dataclass
class ObjData:
    vertices: list[tuple[float, float, float]]  # (x, y, z) coordinates
    faces: list[list[int]]  # Each face is a list of vertex indices

    def write_to_obj(self, file):
        """Write the OBJ data to a file."""
        # Write vertices
        for v in self.vertices:
            file.write(f"v {v[0]} {v[1]} {v[2]}\n")

        # Write faces (OBJ indices are 1-based)
        for face in self.faces:
            indices = " ".join(str(idx + 1) for idx in face)
            file.write(f"f {indices}\n")

    def merge(self, other: "ObjData") -> "ObjData":
        """
        Merge another ObjData instance into this one.

        Args:
            other: The ObjData instance to merge.

        Returns:
            A new ObjData instance with combined vertices and faces.
        """
        vertex_offset = len(self.vertices)
        merged_vertices = self.vertices + other.vertices
        merged_faces = self.faces + [
            [index + vertex_offset for index in face] for face in other.faces
        ]
        return ObjData(merged_vertices, merged_faces)

    def apply_transformation(self, matrix: np.ndarray) -> "ObjData":
        """
        Apply a 4x4 transformation matrix to all vertices in the ObjData.

        Args:
            matrix: A 4x4 transformation matrix.

        Returns:
            A new ObjData instance with transformed vertices.
        """
        transformed_vertices = [
            tuple((matrix @ np.array([x, y, z, 1]))[:3]) for x, y, z in self.vertices
        ]
        return ObjData(transformed_vertices, self.faces)


def parse_clusters(file, verbose: bool = False) -> list[Cluster]:
    """
    Parses the input file and returns a list of Cluster objects.

    Args:
        file: The input file object to read from.
        verbose: If True, print details about each parsed cluster.

    Returns:
        A list of Cluster objects.
    """
    clusters = []
    cluster_pattern = re.compile(
        r"rank (\d+): cluster (\d+) (\w+)\((.*?)\)(?: at angle\((.*?)\))? (?:centered )?(below parent cluster (\d+))?"
    )

    for line in file:
        match = cluster_pattern.match(line.strip())
        if match:
            rank = int(match.group(1))
            cluster_id = int(match.group(2))
            shape_type = match.group(3)
            shape_params = match.group(4)
            angle = float(match.group(5)) if match.group(5) else None
            parent_id = int(match.group(7)) if match.group(7) else None

            # Parse shape
            if shape_type == "cone":
                top_radius, base_radius, height = map(float, shape_params.split(","))
                shape = Cone(top_radius, base_radius, height)
            elif shape_type == "sphere":
                radius = float(shape_params)
                shape = Sphere(radius)
            else:
                raise ValueError(f"Unknown shape type: {shape_type}")

            assert shape is not None, "Shape must be defined"

            # Create Cluster object
            cluster = Cluster(
                shape=shape,
                rank=rank,
                angle=angle,
                parent_id=parent_id,
            )

            # Ensure clusters list is large enough for the current cluster_id
            while len(clusters) <= cluster_id:
                clusters.append(None)
            clusters[cluster_id] = cluster

            if verbose:
                print(f"Parsed cluster {cluster_id}: {cluster}")

    return clusters


def create_translation_matrix(tx: float, ty: float, tz: float) -> np.ndarray:
    """Create a 4x4 translation matrix."""
    return np.array([[1, 0, 0, tx], [0, 1, 0, ty], [0, 0, 1, tz], [0, 0, 0, 1]])


def create_rotation_matrix(
    x_angle: float, y_angle: float, z_angle: float
) -> np.ndarray:
    """Create a 4x4 rotation matrix from Euler angles."""
    rx = np.eye(4)
    ry = np.eye(4)
    rz = np.eye(4)

    # Rotation around X-axis
    cos_x, sin_x = math.cos(x_angle), math.sin(x_angle)
    rx[:3, :3] = [[1, 0, 0], [0, cos_x, -sin_x], [0, sin_x, cos_x]]

    # Rotation around Y-axis
    cos_y, sin_y = math.cos(y_angle), math.sin(y_angle)
    ry[:3, :3] = [[cos_y, 0, sin_y], [0, 1, 0], [-sin_y, 0, cos_y]]

    # Rotation around Z-axis
    cos_z, sin_z = math.cos(z_angle), math.sin(z_angle)
    rz[:3, :3] = [[cos_z, -sin_z, 0], [sin_z, cos_z, 0], [0, 0, 1]]

    # Combine rotations: R = Rz * Ry * Rx
    return rz @ ry @ rx


def apply_transformation(
    matrix: np.ndarray, vector: tuple[float, float, float]
) -> tuple[float, float, float]:
    """Apply a 4x4 transformation matrix to a 3D vector."""
    vec = np.array([*vector, 1])  # Convert to homogeneous coordinates
    transformed_vec = matrix @ vec
    return tuple(transformed_vec[:3])  # Convert back to 3D


def position_clusters(
    clusters: list[Cluster],
    tilt_factor: float = math.radians(45),
    rotate_factor: float = 0.0,
) -> ObjData:
    """
    Position clusters according to their parent-child relationships and angle specifications.
    Children with angles are tilted by the tilt factor.

    Args:
        clusters: List of Cluster objects with shape, parent_id, and angle information
        tilt_factor: Angle in radians to tilt children that have angle specifications
        rotate_factor: Global rotation angle applied at every rank

    Returns:
        ObjData object containing the complete scene with all positioned clusters
    """
    valid_clusters = [c for c in clusters if c is not None]
    sorted_clusters = sorted(valid_clusters, key=lambda c: c.rank)

    # Transformation matrices for each cluster
    transformations = [np.eye(4) for _ in clusters]  # Start with identity matrices
    scene = ObjData([], [])

    for cluster in sorted_clusters:
        # Generate shape model
        if isinstance(cluster.shape, Sphere):
            obj_data = generate_quad_sphere(cluster.shape.radius, 16, 16)
        elif isinstance(cluster.shape, Cone):
            obj_data = generate_cone(
                cluster.shape.top_radius,
                cluster.shape.base_radius,
                cluster.shape.height,
            )
        else:
            raise ValueError(f"Unknown shape type: {type(cluster.shape)}")

        # Default transformation
        transformation = np.eye(4)

        tilt_transformation = np.eye(4)

        if cluster.parent_id is not None:
            parent_idx = cluster.parent_id
            parent_transform = transformations[parent_idx]
            parent = clusters[parent_idx]

            # Ensure parent data exists
            if parent_transform is None or parent is None:
                raise ValueError(f"Parent data for cluster {parent_idx} is missing")

            # Start with parent's transformation
            transformation = parent_transform.copy()

            if cluster.angle is not None:
                angle_rad = math.radians(cluster.angle) + cluster.rank * rotate_factor

                # Apply tilt for clusters with angles
                tilt_x = -tilt_factor * math.sin(angle_rad)
                tilt_z = tilt_factor * math.cos(angle_rad)

                # Create rotation matrix for tilt
                tilt_transformation = create_rotation_matrix(tilt_x, 0, tilt_z)

                # Calculate attachment point based on angle
                if isinstance(parent.shape, Cone):
                    attachment_vector = (
                        parent.shape.base_radius * math.cos(angle_rad),
                        0,  # At base of cone
                        parent.shape.base_radius * math.sin(angle_rad),
                    )
                elif isinstance(parent.shape, Sphere):
                    attachment_vector = (
                        parent.shape.radius * math.cos(angle_rad),
                        0,
                        parent.shape.radius * math.sin(angle_rad),
                    )
                else:
                    raise ValueError(f"Unknown parent shape type: {type(parent.shape)}")

            else:
                # No angle specified, place below parent
                if isinstance(parent.shape, Cone):
                    attachment_vector = (0, 0, 0)
                elif isinstance(parent.shape, Sphere):
                    attachment_vector = (0, -parent.shape.radius, 0)
                else:
                    raise ValueError(f"Unknown parent shape type: {type(parent.shape)}")

            attachment_translation = create_translation_matrix(*attachment_vector)
            offset_translation = np.eye(4)

            # Offset cone clusters by their height
            if isinstance(cluster.shape, Cone):
                # Move down by height of the cone so the top is at the attachment point
                offset_translation = create_translation_matrix(
                    0, -cluster.shape.height, 0
                )

            # Apply translation for attachment point
            transformation = (
                transformation
                @ attachment_translation
                @ tilt_transformation
                @ offset_translation
            )

        # Store the transformation matrix
        idx = clusters.index(cluster)
        transformations[idx] = transformation

        # Apply transformation to the object
        transformed_obj = obj_data.apply_transformation(transformation)

        # Merge into the scene
        scene = scene.merge(transformed_obj)

    return scene


def export_obj(obj_data: ObjData, filename: str) -> None:
    """
    Export ObjData to a .obj file.
    """
    try:
        with open(filename, "w", encoding="utf-8") as file:
            obj_data.write_to_obj(file)
    except IOError as e:
        raise IOError(f"Failed to write OBJ data to {filename}") from e


def generate_quad_sphere(radius: float, x_res: int, y_res: int) -> ObjData:
    """
    Generate a quad sphere by projecting a cube onto a sphere.

    Args:
        radius: The radius of the sphere
        x_res: Number of segments along each edge of the base cube
        y_res: Number of segments along each edge of the base cube

    Returns:
        ObjData object containing vertices and faces for a quad sphere
    """
    vertices = []
    faces = []

    # Function to normalize a point to the sphere surface
    def normalize(x, y, z):
        length = math.sqrt(x * x + y * y + z * z)
        return (radius * x / length, radius * y / length, radius * z / length)

    # Define the six faces of the cube
    # Each face is defined by its normal and two tangent vectors
    faces_data = [
        # normal, u_tangent, v_tangent
        ((1, 0, 0), (0, 0, 1), (0, 1, 0)),  # +X
        ((-1, 0, 0), (0, 0, -1), (0, 1, 0)),  # -X
        ((0, 1, 0), (1, 0, 0), (0, 0, 1)),  # +Y
        ((0, -1, 0), (1, 0, 0), (0, 0, -1)),  # -Y
        ((0, 0, 1), (1, 0, 0), (0, 1, 0)),  # +Z
        ((0, 0, -1), (-1, 0, 0), (0, 1, 0)),  # -Z
    ]

    # Generate vertices for all six faces
    for normal, u_tan, v_tan in faces_data:
        face_start_idx = len(vertices)

        # Generate vertices for this face
        for j in range(y_res + 1):
            for i in range(x_res + 1):
                # Parameter from -1 to 1
                u = (i / x_res) * 2 - 1
                v = (j / y_res) * 2 - 1

                # Vertex on the cube face
                x = normal[0] + u * u_tan[0] + v * v_tan[0]
                y = normal[1] + u * u_tan[1] + v * v_tan[1]
                z = normal[2] + u * u_tan[2] + v * v_tan[2]

                # Project to sphere
                vertices.append(normalize(x, y, z))

        # Generate faces for this cube face
        for j in range(y_res):
            for i in range(x_res):
                v0 = face_start_idx + j * (x_res + 1) + i
                v1 = face_start_idx + j * (x_res + 1) + (i + 1)
                v2 = face_start_idx + (j + 1) * (x_res + 1) + (i + 1)
                v3 = face_start_idx + (j + 1) * (x_res + 1) + i

                # Ensure clockwise winding order for all faces
                if normal[2] == 1 or normal[2] == -1:  # Positive normals
                    faces.append(
                        [v0, v1, v2, v3]
                    )  # Adjust for counter-clockwise normals
                else:  # Negative normals
                    faces.append([v0, v3, v2, v1])  # Clockwise order

    return ObjData(vertices, faces)


def generate_circle_vertices(
    radius: float, y: float, segments: int
) -> List[Tuple[float, float, float]]:
    """
    Generate vertices for a circle in the XZ plane at a given Y coordinate.
    """
    return [
        (
            radius * math.cos(2 * math.pi * i / segments),
            y,
            radius * math.sin(2 * math.pi * i / segments),
        )
        for i in range(segments)
    ]


def generate_cone(
    top_radius: float, base_radius: float, height: float, segments: int = 32
) -> ObjData:
    """
    Generate a cone or truncated cone with the given parameters.

    Args:
        top_radius: Radius of the top circle (0 for a pointed cone)
        base_radius: Radius of the base circle
        height: Height of the cone
        segments: Number of segments around the cone (resolution)

    Returns:
        ObjData object containing vertices and faces for the cone
    """
    vertices: List[Tuple[float, float, float]] = []
    faces: List[List[int]] = []

    # Generate vertices for top circle
    top_indices: List[int] = []
    if top_radius > 0:  # Truncated cone
        # Add top center
        top_center_idx = len(vertices)
        vertices.append((0, height, 0))

        # Add top circle vertices
        top_circle = generate_circle_vertices(top_radius, height, segments)
        top_indices = list(range(len(vertices), len(vertices) + len(top_circle)))
        vertices.extend(top_circle)
    else:  # Pointed cone (top is a single vertex)
        top_center_idx = len(vertices)
        vertices.append((0, height, 0))

    # Add bottom center
    bottom_center_idx = len(vertices)
    vertices.append((0, 0, 0))

    # Generate vertices for bottom circle
    bottom_circle = generate_circle_vertices(base_radius, 0, segments)
    bottom_indices = list(range(len(vertices), len(vertices) + len(bottom_circle)))
    vertices.extend(bottom_circle)

    # Create faces for the bottom (as a fan of triangles)
    for i in range(segments):
        next_i = (i + 1) % segments
        faces.append([bottom_center_idx, bottom_indices[i], bottom_indices[next_i]])

    # Create faces for the top (if truncated)
    if top_radius > 0:
        for i in range(segments):
            next_i = (i + 1) % segments
            faces.append([top_center_idx, top_indices[next_i], top_indices[i]])

    # Create faces for the sides (clockwise order for correct outward normals)
    for i in range(segments):
        next_i = (i + 1) % segments
        if top_radius > 0:
            # If truncated cone, create a quad for each segment
            faces.append(
                [
                    bottom_indices[i],
                    top_indices[i],
                    top_indices[next_i],
                    bottom_indices[next_i],
                ]
            )
        else:
            # If pointed cone, create a triangle for each segment
            faces.append([bottom_indices[i], top_center_idx, bottom_indices[next_i]])

    return ObjData(vertices, faces)


def main():
    # Create argument parser
    parser = argparse.ArgumentParser(
        description="Process input file and write to output file."
    )
    # Add positional arguments for input and output files
    parser.add_argument("input", help="Input file path")
    parser.add_argument("output", help="Output file path")
    parser.add_argument(
        "-v", "--verbose", action="store_true", help="Enable verbose output"
    )
    parser.add_argument(
        "-t",
        "--tilt",
        type=float,
        default=30.0,
        help="Tilt angle in degrees (default: 30.0)",
    )
    parser.add_argument(
        "-r",
        "--rotation",
        type=float,
        default=0.0,
        help="Global rotation angle in degrees (default: 0.0)",
    )

    # Parse arguments
    args = parser.parse_args()

    with open(args.input, "r", encoding="utf-8") as infile:
        clusters = parse_clusters(infile, verbose=args.verbose)
        obj_data = position_clusters(
            clusters,
            tilt_factor=math.radians(args.tilt),
            rotate_factor=math.radians(args.rotation),
        )
        export_obj(obj_data, args.output)

    return 0


if __name__ == "__main__":
    sys.exit(main())
