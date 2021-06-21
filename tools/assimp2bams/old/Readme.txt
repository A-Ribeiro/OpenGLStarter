Binary Asilva Mesh Scene - BAMS


assimp2bams -x line4pts -m <mesh name> -o file.bams -- extract lineset

assimp2bams -x vertex -m <mesh name> -o file.bams -- extract vertex pts

assimp2bams -x mesh3d -s -- extract all mesh3d and export scene definition

assimp2bams -x mesh3d -s -m -- extract mesh3d scene and materials

-- extract all things as line4pts
--  static bake (make final all transforms)
--  export non-indexed primitive
assimp2bams -x line4pts -s bake-static -i none <assimp model> -o <outfile>.bams

x: what to extract
  - line4pts
  - vertex
  - mesh3d

s: node scene options
  - bake-static
  - graph-complete

i: index mode
  - none
  - strip
  - regular

o: output

