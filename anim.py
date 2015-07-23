import vtk
import math

#rotations=[
#	[1, 0, 0, 0],
#	[0.707, 0, -0.707, 0],
#	[0.707, -0.707, 0, 0],
#	[0.707, 0, 0, -0.707],
#]

rotations=[
	[0,0,0],
	[90,0,0],
	[0,90,0],
	[0,0,90],
]

device = open('/dev/ttyACM0','r+b')
 
class vtkTimerCallback():
	def __init__(self):
		self.timer_count = 0

	def execute(self,obj,event):
		print self.timer_count
#		self.actor.SetPosition(self.timer_count, self.timer_count,self.timer_count);
#		w = rotations[self.timer_count % 4][0]
#		x = rotations[self.timer_count % 4][1]
#		y = rotations[self.timer_count % 4][2]
#		z = rotations[self.timer_count % 4][3]
#		self.actor.RotateWXYZ(w,x,y,z)
		device.write(":0\n")
		l = device.readline().split(',')
		q0 = float(l[0])
		q1 = float(l[1])
		q2 = float(l[2])
		q3 = float(l[3])
		phi = math.atan2(2*(q0*q1+q2*q3),1-2*(q1*q1+q2*q2))
		theta = math.asin(2*(q0*q2-q3*q1))
		psi = math.atan2(2*(q0*q3+q1*q2),1-2*(q2*q2+q3*q3))
		phi = phi / math.pi * 180
		theta = theta / math.pi * 180
		psi = psi / math.pi * 180
#		print phi,theta,psi
#		self.actor.SetOrientation(rotations[self.timer_count % 4])
		self.actor.SetOrientation(phi,theta,psi)
		iren = obj
		iren.GetRenderWindow().Render()
		self.timer_count += 1


def main():
#Create a sphere
#	sphereSource = vtk.vtkSphereSource()
#	sphereSource.SetCenter(0.0, 0.0, 0.0)
#	sphereSource.SetRadius(5)
	sphereSource = vtk.vtkCubeSource()
	sphereSource.SetBounds(-1,1,-2,2,-3,3)

#Create a mapper and actor
	mapper = vtk.vtkPolyDataMapper()
	mapper.SetInputConnection(sphereSource.GetOutputPort())
	actor = vtk.vtkActor()
	actor.SetMapper(mapper)
	prop = actor.GetProperty()

# Setup a renderer, render window, and interactor
	renderer = vtk.vtkRenderer()
	renderWindow = vtk.vtkRenderWindow()
#renderWindow.SetWindowName("Test")

	renderWindow.AddRenderer(renderer);
	renderWindowInteractor = vtk.vtkRenderWindowInteractor()
	renderWindowInteractor.SetRenderWindow(renderWindow)

#Add the actor to the scene
	renderer.AddActor(actor)
	renderer.SetBackground(0,0,0) # Background color white

#Render and interact
	renderWindow.Render()

# Initialize must be called prior to creating timer events.
	renderWindowInteractor.Initialize()

# Sign up to receive TimerEvent
	cb = vtkTimerCallback()
	cb.actor = actor
	renderWindowInteractor.AddObserver('TimerEvent', cb.execute)
	timerId = renderWindowInteractor.CreateRepeatingTimer(200);

#start the interaction and timer
	renderWindowInteractor.Start()


if __name__ == '__main__':
	main()
