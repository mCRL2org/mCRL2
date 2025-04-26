#ifndef APPLICATIONFUNCTIONS_H
#define APPLICATIONFUNCTIONS_H

namespace ApplicationFunctions
{
  inline void apply_forces(QVector3D& pos, 
                           const QVector3D& f,
                           const float speed)
  {
    const float scaling = 0.0050f;
    pos = pos + (f * speed * scaling ); 
  }
}; // namespace ApplicationFunctions

#endif
