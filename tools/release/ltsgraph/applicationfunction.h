#ifndef APPLICATIONFUNCTIONS_H
#define APPLICATIONFUNCTIONS_H

namespace ApplicationFunctions
{
  inline void apply_forces(QVector3D& pos, 
                           const QVector3D& f,
                           const float speed,
                           const float temperature)
  {
    const float scaling = 0.0050f;
    pos = pos + (f * speed * scaling * temperature ); 
  }
}; // namespace ApplicationFunctions

#endif
