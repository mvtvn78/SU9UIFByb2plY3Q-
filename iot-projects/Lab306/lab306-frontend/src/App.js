import React, { useEffect, useState } from 'react';
import axios from 'axios';
import {
Box, Button, Card, CardContent, Container, Divider,
TextField, Typography, Dialog, DialogTitle,
DialogContent, DialogActions
} from '@mui/material';
import SendIcon from '@mui/icons-material/Send';
import AddIcon from '@mui/icons-material/Add';
import VisibilityIcon from '@mui/icons-material/Visibility';
function App() {
const [devices, setDevices] = useState([]);
const [newDevice, setNewDevice] = useState({ name: '', topic: '' });
const [payloads, setPayloads] = useState({});
const [telemetry, setTelemetry] = useState([]);
const [selectedDevice, setSelectedDevice] = useState(null);
const [openDialog, setOpenDialog] = useState(false);
useEffect(() => {
fetchDevices();
}, []);
const fetchDevices = async () => {
const res = await axios.get('http://localhost:8080/devices');
setDevices(res.data);
const initPayloads = {};
res.data.forEach(d => initPayloads[d.id] = '');
setPayloads(initPayloads);
};
const handleSend = async (id) => {
const payload = payloads[id];
if (!payload) return;
await axios.post(`http://localhost:8080/devices/${id}/control`, payload, {
headers: { 'Content-Type': 'text/plain' }
});
alert('Lệnh đã gửi!');
};
const handleCreate = async () => {
if (!newDevice.name || !newDevice.topic) return;
await axios.post('http://localhost:8080/devices', newDevice);
setNewDevice({ name: '', topic: '' });
fetchDevices();
};
const fetchTelemetry = async (deviceId) => {
const res = await axios.get(`http://localhost:8080/telemetry/${deviceId}`);
return res.data;
};
const handleViewTelemetry = async (device) => {
const data = await fetchTelemetry(device.id);
setTelemetry(data);
setSelectedDevice(device);
setOpenDialog(true);
};
return (
<Container maxWidth="sm" sx={{ mt: 4 }}>
<Typography variant="h4" textAlign="center" fontWeight="bold" gutterBottom>
📡 IoT Device Dashboard
</Typography>
<Typography variant="h6" gutterBottom>📋 Danh sách thiết bị</Typography>
{devices.map(device => (
<Card key={device.id} sx={{ mb: 2, backgroundColor: '#f0f4ff' }}>
<CardContent>
<Typography fontWeight="bold">{device.name}</Typography>
<Typography variant="body2" gutterBottom>MQTT Topic: {device.topic}</Typography>
<TextField
fullWidth
label="Lệnh điều khiển"
placeholder="{data:20}"
multiline
maxRows={3}
value={payloads[device.id]}
onChange={(e) => setPayloads({ ...payloads, [device.id]: e.target.value })}
sx={{ mt: 1, mb: 2 }}
/>
{/* ✅ Hai nút nằm cùng dòng, giống Flutter */}
<Box display="flex" justifyContent="flex-end" gap={1}>
<Button
variant="contained"
onClick={() => handleSend(device.id)}
endIcon={<SendIcon />}
sx={{ textTransform: 'none' }}
>
GỬI LỆNH
</Button>
<Button
variant="outlined"
onClick={() => handleViewTelemetry(device)}
startIcon={<VisibilityIcon />}
sx={{ textTransform: 'none' }}
>
XEM DỮ LIỆU
</Button>
</Box>
</CardContent>
</Card>
))}
<Divider sx={{ my: 3 }} />
<Typography variant="h6" gutterBottom>➕ Thêm thiết bị mới</Typography>
<TextField
fullWidth
label="Tên thiết bị"
variant="outlined"
sx={{ mb: 2 }}
value={newDevice.name}
onChange={(e) => setNewDevice({ ...newDevice, name: e.target.value })}
/>
<TextField
fullWidth
label="Topic MQTT"
variant="outlined"
sx={{ mb: 2 }}
value={newDevice.topic}
onChange={(e) => setNewDevice({ ...newDevice, topic: e.target.value })}
/>
<Button
variant="contained"
fullWidth
onClick={handleCreate}
startIcon={<AddIcon />}
sx={{ textTransform: 'none' }}
>
TẠO THIẾT BỊ
</Button>
{/* Dialog hiển thị telemetry */}
{selectedDevice && (
<Dialog open={openDialog} onClose={() => setOpenDialog(false)} fullWidth maxWidth="sm">
<DialogTitle>Telemetry - {selectedDevice.name}</DialogTitle>
<DialogContent dividers>
{telemetry.length === 0 ? (
<Typography>Không có dữ liệu</Typography>
) : (
telemetry.map((t, i) => (
<Box key={i} sx={{ mb: 1 }}>
<Typography><b>Giá trị:</b> {t.value}</Typography>
<Typography variant="caption" color="text.secondary">{t.timestamp}</Typography>
</Box>
))
)}
</DialogContent>
<DialogActions>
<Button onClick={() => setOpenDialog(false)}>Đóng</Button>
</DialogActions>
</Dialog>
)}
</Container>
);
}
export default App;